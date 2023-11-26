// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNScope.h"
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNDecompositionResult.h"
#include "Domain/Interpreter/HTNNodeResult.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Domain/Printer/HTNDecompositionHelpers.h"
#include "Domain/Printer/HTNDecompositionPrinterContext.h"
#include "Domain/Printer/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Printer/HTNNodeState.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
enum HTNValueExpressionNodeResult : uint8
{
    STRING,
    COLOR
};

void PrePrintChoicePointNode(const std::string& inNodePath, const size inLastDecompositionStep,
                             HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNChoicePointNodeState* ChoicePointNodeState = ioDecompositionPrinterContext.FindChoicePointNodeStateMutable(inNodePath);
    if (ChoicePointNodeState)
    {
        const bool ShouldRefreshNodeStates = ioDecompositionPrinterContext.ShouldRefreshNodeStates();
        if (!ShouldRefreshNodeStates)
        {
            return;
        }

        const int32 MinDecompositionStep = ioDecompositionPrinterContext.GetMinDecompositionStep();
        const int32 MaxDecompositionStep = ioDecompositionPrinterContext.GetMaxDecompositionStep();
        const int32 DecompositionStep    = ChoicePointNodeState->FindOpenDecompositionStepInRange(MinDecompositionStep, MaxDecompositionStep);
        ChoicePointNodeState->SetDecompositionStep(DecompositionStep);
    }
    else
    {
        // Default open last decomposition step
        const int32           DecompositionStep = static_cast<const int32>(inLastDecompositionStep);
        static constexpr bool IsNodeOpen        = true;
        ioDecompositionPrinterContext.AddChoicePointNodeState(inNodePath, HTNChoicePointNodeState(DecompositionStep, IsNodeOpen));
        ioDecompositionPrinterContext.RefreshNodeStates();
    }
}

void PrePrintRegularNode(const std::string& inNodePath, const HTNNodeResultStepsCollection& inNodeResultStepsCollection,
                         HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNNodeState* NodeState = ioDecompositionPrinterContext.FindNodeStateMutable(inNodePath);
    if (NodeState)
    {
        const bool ShouldRefreshNodeStates = ioDecompositionPrinterContext.ShouldRefreshNodeStates();
        if (!ShouldRefreshNodeStates)
        {
            return;
        }

        const int32 CurrentDecompositionStep        = ioDecompositionPrinterContext.GetCurrentDecompositionStep();
        const bool  IsCurrentDecompositionStepValid = ioDecompositionPrinterContext.IsCurrentDecompositionStepValid();
        if (IsCurrentDecompositionStepValid)
        {
            const auto NodeResultStepsCollectionIt = inNodeResultStepsCollection.find(CurrentDecompositionStep);
            if (NodeResultStepsCollectionIt != inNodeResultStepsCollection.end())
            {
                const HTNNodeResultCollection& NodeResultCollection   = NodeResultStepsCollectionIt->second;
                const auto                     NodeResultCollectionIt = NodeResultCollection.find(HTNNodeStep::END);
                if (NodeResultCollectionIt != NodeResultCollection.end())
                {
                    NodeState->SetDecompositionStep(CurrentDecompositionStep);
                }
            }
        }
        else
        {
            NodeState->SetDecompositionStep(CurrentDecompositionStep);
        }
    }
    else
    {
        // Default open current decomposition step
        static constexpr int32 DecompositionStep = HTNDecompositionHelpers::kInvalidDecompositionStep;
        static constexpr bool  IsNodeOpen        = true;
        ioDecompositionPrinterContext.AddNodeState(inNodePath, HTNNodeState(DecompositionStep, IsNodeOpen));
        ioDecompositionPrinterContext.RefreshNodeStates();
    }
}

void PrePrintNode(const std::string& inNodePath, const bool inIsChoicePoint, const HTNNodeResultStepsCollection& inNodeResultStepsCollection,
                  const size inLastDecompositionStep, HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    if (inIsChoicePoint)
    {
        PrePrintChoicePointNode(inNodePath, inLastDecompositionStep, ioDecompositionPrinterContext);
    }
    else
    {
        PrePrintRegularNode(inNodePath, inNodeResultStepsCollection, ioDecompositionPrinterContext);
    }
}

void PostPrintChoicePointNode(const std::string& inNodePath, const bool inIsNodeOpen, HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNChoicePointNodeState& ChoicePointNodeState = ioDecompositionPrinterContext.GetChoicePointNodeStateMutable(inNodePath);

    const int32 PreviousDecompositionStep = ChoicePointNodeState.GetDecompositionStep();

    const int32 CurrentDecompositionStep = ioDecompositionPrinterContext.GetCurrentDecompositionStep();
    ChoicePointNodeState.SetDecompositionStep(CurrentDecompositionStep);

    const bool IsCurrentDecompositionStepValid = ioDecompositionPrinterContext.IsCurrentDecompositionStepValid();
    if (IsCurrentDecompositionStepValid)
    {
        ChoicePointNodeState.SetIsOpen(CurrentDecompositionStep, inIsNodeOpen);
    }
    else
    {
        ChoicePointNodeState.SetIsOpen(PreviousDecompositionStep, inIsNodeOpen);
    }
}

void PostPrintRegularNode(const std::string& inNodePath, const bool inIsNodeOpen, const HTNNodeResultStepsCollection& inNodeResultStepsCollection,
                          HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{

    HTNNodeState& NodeState = ioDecompositionPrinterContext.GetNodeStateMutable(inNodePath);

    const int32 CurrentDecompositionStep = ioDecompositionPrinterContext.GetCurrentDecompositionStep();

    const bool IsCurrentDecompositionStepValid = ioDecompositionPrinterContext.IsCurrentDecompositionStepValid();
    if (IsCurrentDecompositionStepValid)
    {
        const auto NodeResultStepsCollectionIt = inNodeResultStepsCollection.find(CurrentDecompositionStep);
        if (NodeResultStepsCollectionIt != inNodeResultStepsCollection.end())
        {
            const HTNNodeResultCollection& NodeResultCollection   = NodeResultStepsCollectionIt->second;
            const auto                     NodeResultCollectionIt = NodeResultCollection.find(HTNNodeStep::END);
            if (NodeResultCollectionIt != NodeResultCollection.end())
            {
                NodeState.SetDecompositionStep(CurrentDecompositionStep);
            }
        }
    }
    else
    {
        NodeState.SetDecompositionStep(CurrentDecompositionStep);
    }

    NodeState.SetIsOpen(inIsNodeOpen);
}

void PostPrintNode(const std::string& inNodePath, const bool inIsChoicePoint, const bool inIsNodeOpen,
                   const HTNNodeResultStepsCollection& inNodeResultStepsCollection, HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    const bool ShouldRefreshNodeStates = ioDecompositionPrinterContext.ShouldRefreshNodeStates();
    if (!ShouldRefreshNodeStates)
    {
        return;
    }

    if (inIsChoicePoint)
    {
        PostPrintChoicePointNode(inNodePath, inIsNodeOpen, ioDecompositionPrinterContext);
    }
    else
    {
        PostPrintRegularNode(inNodePath, inIsNodeOpen, inNodeResultStepsCollection, ioDecompositionPrinterContext);
    }
}

HTN_NODISCARD bool IsChoicePointNodeValid(const size inDecompositionStep, const int32 inCurrentDecompositionStep, const int32 inMinDecompositionStep,
                                          const int32 inMaxDecompositionStep)
{
    // Filter available nodes within range [min, max)
    if (!HTNDecompositionHelpers::IsDecompositionStepInRange(static_cast<const int32>(inDecompositionStep), inMinDecompositionStep,
                                                             inMaxDecompositionStep))
    {
        return false;
    }

    if (HTNDecompositionHelpers::IsDecompositionStepValid(inCurrentDecompositionStep))
    {
        if (static_cast<const int32>(inDecompositionStep) != inCurrentDecompositionStep)
        {
            return false;
        }
    }

    return true;
}

HTN_NODISCARD bool IsRegularNodeValid(const size inDecompositionStep, const int32 inCurrentDecompositionStep, const int32 inMinDecompositionStep,
                                      const int32 inMaxDecompositionStep)
{
    // Filter available nodes within range [min, max)
    if (!HTNDecompositionHelpers::IsDecompositionStepInRange(static_cast<const int32>(inDecompositionStep), inMinDecompositionStep,
                                                             inMaxDecompositionStep))
    {
        return false;
    }

    if (static_cast<const int32>(inDecompositionStep) != inCurrentDecompositionStep)
    {
        return false;
    }

    return true;
}

HTN_NODISCARD bool IsNodeValid(const size inDecompositionStep, const bool inIsChoicePoint, const int32 inCurrentDecompositionStep,
                               const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep)
{
    return inIsChoicePoint ? IsChoicePointNodeValid(inDecompositionStep, inCurrentDecompositionStep, inMinDecompositionStep, inMaxDecompositionStep)
                           : IsRegularNodeValid(inDecompositionStep, inCurrentDecompositionStep, inMinDecompositionStep, inMaxDecompositionStep);
}

HTN_NODISCARD HTNDecompositionPrinterContext& GetDecompositionPrinterContext(HTNNodeVisitorContextBase& ioDecompositionPrinterContext)
{
    return static_cast<HTNDecompositionPrinterContext&>(ioDecompositionPrinterContext);
}
} // namespace

bool HTNDecompositionPrinter::Print(HTNDecompositionPrinterContext& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("PrintDecomposition");

    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDecompositionPrinterContext.GetDomainNode();
    if (!DomainNode)
    {
        HTN_LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode, ioDecompositionPrinterContext).GetValue<bool>();
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetDomainNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    const std::string DomainNodeID                = inDomainNode.GetID();
    HTNPathManager&   CurrentNodePathManager      = DecompositionPrinterContext.GetCurrentNodePathManagerMutable();
    const HTNScope    DomainNodeScope             = HTNScope(DomainNodeID, CurrentNodePathManager);
    HTNPathManager&   CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManagerMutable();
    const HTNScope    DomainVariablesScope        = HTNScope(DomainNodeID, CurrentVariablesPathManager);

    // Top-level compound task node
    const std::string&                               EntryPointID             = DecompositionPrinterContext.GetEntryPointID();
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDomainHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    return GetNodeValue(*TopLevelCompoundTaskNode, ioDecompositionPrinterContext).GetValue<bool>();
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetAxiomNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManagerMutable();
    const HTNScope  AxiomVariablesScope         = HTNScope(inAxiomNode.GetID(), CurrentVariablesPathManager);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&            IDNode         = inAxiomNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        static const std::string Prefix = "";

        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        PrintColoredValueExpressionNode(IDNode, Prefix, IDColor, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ParameterNode, Prefix, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
        {
            GetNodeValue(*ConditionNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ParameterNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAxiomNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetMethodNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManagerMutable();
    const HTNScope  MethodVariablesScope        = HTNScope(inMethodNode.GetID(), CurrentVariablesPathManager);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&            IDNode         = inMethodNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](HTN_MAYBE_UNUSED const HTNNodeResult& inNodeResult,
                                                       HTN_MAYBE_UNUSED const HTNNodeStep    inNodeStep) {
        static const std::string Prefix = "";

        PrintValueExpressionNode(IDNode, Prefix, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ParameterNode, Prefix, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            GetNodeValue(*BranchNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ParameterNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inMethodNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetBranchNodeValue");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inBranchNode.GetIDNode();

    const HTNNodeTitleFunction NodeTitleFunction = [&](HTN_MAYBE_UNUSED const HTNNodeResult& inNodeResult,
                                                       HTN_MAYBE_UNUSED const HTNNodeStep    inNodeStep) {
        static const std::string Prefix = "";
        PrintValueExpressionNode(IDNode, Prefix, ioDecompositionPrinterContext);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
        {
            GetNodeValue(*PreConditionNode, ioDecompositionPrinterContext);
        }

        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
        for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
        {
            GetNodeValue(*TaskNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_None;
    return PrintNode(inBranchNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetConditionNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             IDNode        = inConditionNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        static const std::string Prefix = "";

        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        PrintColoredValueExpressionNode(IDNode, Prefix, IDColor, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ArgumentNode, Prefix, ioDecompositionPrinterContext);
        }
    };

    const HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManager();

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ArgumentNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    const HTNNodeBehaviorFunction       NodeBehaviorFunction;
    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode,
                                       HTNNodeVisitorContextBase&   ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetAxiomConditionNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             IDNode        = inAxiomConditionNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        static const std::string IDPrefix = "#";
        const ImVec4             IDColor  = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        PrintColoredValueExpressionNode(IDNode, IDPrefix, IDColor, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            static const std::string ArgumentPrefix = "";
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ArgumentNode, ArgumentPrefix, ioDecompositionPrinterContext);
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::string AxiomNodeID =
            GetNodeValue(*IDNode, ioDecompositionPrinterContext).GetListElement(HTNValueExpressionNodeResult::STRING).GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode> AxiomNode = DomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode, ioDecompositionPrinterContext);
    };

    const HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManager();

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ArgumentNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNode(inAxiomConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags,
                     ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetAndConditionNodeValue");

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        const std::string IDString = "and";
        const ImVec4      IDColor  = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAndConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetOrConditionNodeValue");

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        const std::string IDString = "or";
        const ImVec4      IDColor  = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inOrConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetAltConditionNodeValue");

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        const std::string IDString = "alt";
        const ImVec4      IDColor  = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDecompositionPrinterContext);
        }
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAltConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetNotConditionNodeValue");

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep) {
        const std::string IDString = "not";
        const ImVec4      IDColor  = HTNImGuiHelpers::GetNodeColor(inNodeResult, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode, ioDecompositionPrinterContext);
    };

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inNotConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetCompoundTaskNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             IDNode        = inCompoundTaskNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](HTN_MAYBE_UNUSED const HTNNodeResult& inNodeResult,
                                                       HTN_MAYBE_UNUSED const HTNNodeStep    inNodeStep) {
        static const std::string Prefix = "";

        PrintValueExpressionNode(IDNode, Prefix, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ArgumentNode, Prefix, ioDecompositionPrinterContext);
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::string MethodNodeID =
            GetNodeValue(*IDNode, ioDecompositionPrinterContext).GetListElement(HTNValueExpressionNodeResult::STRING).GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode> MethodNode = DomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode, ioDecompositionPrinterContext);
    };

    const HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManager();

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ArgumentNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNode(inCompoundTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags, ioDecompositionPrinterContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode,
                                       HTNNodeVisitorContextBase&  ioDecompositionPrinterContext) const
{
    OPTICK_EVENT("GetPrimitiveTaskNodeValue");

    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>&             IDNode        = inPrimitiveTaskNode.GetIDNode();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](HTN_MAYBE_UNUSED const HTNNodeResult& inNodeResult,
                                                       HTN_MAYBE_UNUSED const HTNNodeStep    inNodeStep) {
        static const std::string IDPrefix = "!";
        PrintValueExpressionNode(IDNode, IDPrefix, ioDecompositionPrinterContext);

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            static const std::string ArgumentPrefix = "";
            ImGui::SameLine();
            PrintColoredValueExpressionNode(ArgumentNode, ArgumentPrefix, ioDecompositionPrinterContext);
        }
    };

    const HTNPathManager& CurrentVariablesPathManager = DecompositionPrinterContext.GetCurrentVariablesPathManager();

    const HTNNodeInstanceFunction NodeInstanceFunction = [&](const HTNNodeResult& inNodeResult, const std::string& inNodeLabel) -> HTNNodeInstance {
        return HTNNodeInstance(inNodeResult, ArgumentNodes, CurrentVariablesPathManager, inNodeLabel);
    };

    const HTNNodeBehaviorFunction       NodeBehaviorFunction;
    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inPrimitiveTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeInstanceFunction, TreeNodeFlags,
                     ioDecompositionPrinterContext);
}

bool HTNDecompositionPrinter::PrintNode(const HTNNodeBase& inNode, const HTNNodeTitleFunction inNodeTitleFunction,
                                        const HTNNodeBehaviorFunction inNodeBehaviorFunction, const HTNNodeInstanceFunction inNodeInstanceFunction,
                                        const ImGuiTreeNodeFlags inTreeNodeFlags, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioDecompositionPrinterContext);

    HTNPathManager& CurrentNodePathManager = DecompositionPrinterContext.GetCurrentNodePathManagerMutable();
    const HTNScope  NodeScope              = HTNScope(inNode.GetID(), CurrentNodePathManager);

    const HTNDecompositionResult& DecompositionResult = DecompositionPrinterContext.GetDecompositionResult();
    const std::string&            CurrentNodePath     = CurrentNodePathManager.GetPath();
    const HTNNodeResultHistory*   NodeResultHistory   = DecompositionResult.FindNodeResultHistory(CurrentNodePath);
    if (!NodeResultHistory)
    {
        return false;
    }

    const bool                          IsChoicePoint             = NodeResultHistory->IsChoicePoint();
    const HTNNodeResultStepsCollection& NodeResultStepsCollection = NodeResultHistory->GetNodeResultStepsCollection();

    const auto LastIt                = NodeResultStepsCollection.rbegin();
    const size LastDecompositionStep = LastIt->first;

    const bool IsDecompositionSuccessful = DecompositionPrinterContext.IsDecompositionSuccessful();

    PrePrintNode(CurrentNodePath, IsChoicePoint, NodeResultStepsCollection, LastDecompositionStep, DecompositionPrinterContext);

    const HTNNodeStep CurrentNodeStep              = DecompositionPrinterContext.GetNodeStep(CurrentNodePath, IsChoicePoint);
    const int32       CurrentNodeDecompositionStep = DecompositionPrinterContext.GetNodeDecompositionStep(CurrentNodePath, IsChoicePoint);
    const bool        IsCurrentNodeOpen = DecompositionPrinterContext.IsNodeOpen(CurrentNodePath, CurrentNodeDecompositionStep, IsChoicePoint);

    const int32 MinDecompositionStep   = DecompositionPrinterContext.GetMinDecompositionStep();
    const int32 MaxDecompositionStep   = DecompositionPrinterContext.GetMaxDecompositionStep();
    const bool  ShouldIgnoreImGuiState = DecompositionPrinterContext.ShouldIgnoreImGuiState();
    const bool  IsCurrentNodeVisible   = DecompositionPrinterContext.IsCurrentNodeVisible();

    bool NewIsCurrentNodeOpen = IsCurrentNodeOpen;

    for (auto It = NodeResultStepsCollection.begin(); It != NodeResultStepsCollection.end(); ++It)
    {
        const size DecompositionStep = It->first;
        if (!IsNodeValid(DecompositionStep, IsChoicePoint, CurrentNodeDecompositionStep, MinDecompositionStep, MaxDecompositionStep))
        {
            continue;
        }

        const HTNNodeResultCollection& NodeResultCollection = It->second;
        const HTNNodeResult&           NodeResult           = NodeResultCollection.at(CurrentNodeStep);

        const std::string NodeLabel =
            IsChoicePoint ? std::format("##{}{}", CurrentNodePath, DecompositionStep) : std::format("##{}", CurrentNodePath);
        const HTNNodeInstance NodeInstance  = inNodeInstanceFunction ? inNodeInstanceFunction(NodeResult, NodeLabel) : HTNNodeInstance();
        ImGuiTreeNodeFlags    TreeNodeFlags = inTreeNodeFlags | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        const bool IsNodeSelected = DecompositionPrinterContext.IsNodeSelected(NodeLabel);
        if (IsNodeSelected)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;

            DecompositionPrinterContext.SelectNodeInstance(NodeInstance);
        }

        if (IsChoicePoint)
        {
            // Display arrow
            TreeNodeFlags &= ~ImGuiTreeNodeFlags_Leaf;
        }

        bool IsOpen = false;
        if (IsCurrentNodeVisible)
        {
            if (IsChoicePoint)
            {
                NewIsCurrentNodeOpen =
                    DecompositionPrinterContext.IsNodeOpen(CurrentNodePath, static_cast<const int32>(DecompositionStep), IsChoicePoint);
            }

            HTNImGuiHelpers::SetTreeNodeOpen(NodeLabel, NewIsCurrentNodeOpen);

            const bool WasCurrentNodeOpen = NewIsCurrentNodeOpen;

            if (IsChoicePoint)
            {
                // Push arrow color
                const bool   IsSuccessful = IsDecompositionSuccessful && (LastDecompositionStep == DecompositionStep);
                const ImVec4 ArrowColor   = IsSuccessful ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
                ImGui::PushStyleColor(ImGuiCol_Text, ArrowColor);
            }

            IsOpen = ImGui::TreeNodeEx(NodeLabel.c_str(), TreeNodeFlags);

            if (IsChoicePoint)
            {
                // Pop arrow color
                ImGui::PopStyleColor(1);
            }

            if (!ShouldIgnoreImGuiState)
            {
                NewIsCurrentNodeOpen = IsOpen;
            }

            if (WasCurrentNodeOpen != NewIsCurrentNodeOpen)
            {
                DecompositionPrinterContext.RefreshNodeStates();
            }

            static constexpr ImGuiHoveredFlags LineHoveredFlags = ImGuiHoveredFlags_ForTooltip;
            if (ImGui::IsItemHovered(LineHoveredFlags))
            {
                // Watch tooltip
                const std::shared_ptr<const HTNDomainNode>& DomainNode  = DecompositionPrinterContext.GetDomainNode();
                const HTNDecompositionTooltipMode           TooltipMode = DecompositionPrinterContext.GetTooltipMode();
                HTNDecompositionWatchTooltipPrinterContext  DecompositionWatchTooltipPrinterContext =
                    HTNDecompositionWatchTooltipPrinterContext(DomainNode, NodeInstance, TooltipMode);
                mDecompositionWatchTooltipPrinter.Print(DecompositionWatchTooltipPrinterContext);
            }

            if (HTNImGuiHelpers::IsCurrentItemSelected())
            {
                DecompositionPrinterContext.SelectNodeInstance(NodeInstance);
            }

            if (IsChoicePoint)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("%i", DecompositionStep);
            }

            if (inNodeTitleFunction)
            {
                ImGui::SameLine();
                inNodeTitleFunction(NodeResult, CurrentNodeStep);
            }
        }

        // Choice point determines decomposition step
        if (IsChoicePoint)
        {
            const int32 NewCurrentDecompositionStep =
                NewIsCurrentNodeOpen ? static_cast<const int32>(DecompositionStep) : HTNDecompositionHelpers::kInvalidDecompositionStep;
            DecompositionPrinterContext.SetCurrentDecompositionStep(NewCurrentDecompositionStep);

            // Set range to filter nodes
            const int32 NewMinDecompositionStep = NewIsCurrentNodeOpen ? NewCurrentDecompositionStep : std::numeric_limits<int32>::max();
            DecompositionPrinterContext.SetMinDecompositionStep(NewMinDecompositionStep);

            auto NextIt = It;
            ++NextIt;
            const int32 NewMaxDecompositionStep =
                NewIsCurrentNodeOpen
                    ? ((NodeResultStepsCollection.end() != NextIt) ? static_cast<const int32>(NextIt->first) : std::numeric_limits<int32>::max())
                    : std::numeric_limits<int32>::min();
            DecompositionPrinterContext.SetMaxDecompositionStep(NewMaxDecompositionStep);
        }

        // Grouping point determines visibility
        const bool IsGroupingPoint = !(TreeNodeFlags & ImGuiTreeNodeFlags_Leaf);
        if (IsGroupingPoint)
        {
            if (!NewIsCurrentNodeOpen)
            {
                static constexpr bool NewIsCurrentNodeVisible = false;
                DecompositionPrinterContext.SetIsCurrentNodeVisible(NewIsCurrentNodeVisible);
            }
        }

        // Perform selected node behavior
        if (inNodeBehaviorFunction)
        {
            inNodeBehaviorFunction();
        }

        // Grouping point determines visibility
        if (IsGroupingPoint)
        {
            if (!IsChoicePoint)
            {
                DecompositionPrinterContext.SetIsCurrentNodeVisible(IsCurrentNodeVisible);
            }
        }

        if (IsCurrentNodeVisible)
        {
            if (IsOpen)
            {
                if (!(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
                {
                    ImGui::TreePop();
                }
            }
        }

        if (NewIsCurrentNodeOpen)
        {
            break;
        }
    }

    PostPrintNode(CurrentNodePath, IsChoicePoint, NewIsCurrentNodeOpen, NodeResultStepsCollection, DecompositionPrinterContext);

    return true;
}
#endif
