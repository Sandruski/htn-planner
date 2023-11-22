// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionPrinterContext.h"
#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNScope.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
HTNDecompositionPrinterContext& GetDecompositionPrinterContext(HTNNodeVisitorContextBase& ioContext)
{
    return static_cast<HTNDecompositionPrinterContext&>(ioContext);
}

void PrePrintChoicePointNode(const std::string& inNodePath, const size inLastDecompositionStep,
                             HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNDecompositionChoicePointNodeState* ChoicePointNodeState = ioDecompositionPrinterContext.FindChoicePointNodeStateMutable(inNodePath);
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
        // Default open successful decomposition step
        const int32           DecompositionStep = static_cast<const int32>(inLastDecompositionStep);
        static constexpr bool IsNodeOpen        = true;
        ioDecompositionPrinterContext.AddChoicePointNodeState(inNodePath, HTNDecompositionChoicePointNodeState(DecompositionStep, IsNodeOpen));
        ioDecompositionPrinterContext.RefreshNodeStates();
    }
}

void PrePrintRegularNode(const std::string& inNodePath, const HTNNodeSnapshotStepsCollectionDebug& inNodeSnapshotStepsCollection,
                         HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNDecompositionNodeState* NodeState = ioDecompositionPrinterContext.FindNodeStateMutable(inNodePath);
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
            const auto NodeSnapshotStepsCollectionIt = inNodeSnapshotStepsCollection.find(CurrentDecompositionStep);
            if (NodeSnapshotStepsCollectionIt != inNodeSnapshotStepsCollection.end())
            {
                const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection   = NodeSnapshotStepsCollectionIt->second;
                const auto                            NodeSnapshotCollectionIt = NodeSnapshotCollection.find(HTNNodeStep::END);
                if (NodeSnapshotCollectionIt != NodeSnapshotCollection.end())
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
        ioDecompositionPrinterContext.AddNodeState(inNodePath, HTNDecompositionNodeState(DecompositionStep, IsNodeOpen));
        ioDecompositionPrinterContext.RefreshNodeStates();
    }
}

void PrePrintNode(const std::string& inNodePath, const bool inIsChoicePoint, const HTNNodeSnapshotStepsCollectionDebug& inNodeSnapshotStepsCollection,
                  const size inLastDecompositionStep, HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    if (inIsChoicePoint)
    {
        PrePrintChoicePointNode(inNodePath, inLastDecompositionStep, ioDecompositionPrinterContext);
    }
    else
    {
        PrePrintRegularNode(inNodePath, inNodeSnapshotStepsCollection, ioDecompositionPrinterContext);
    }
}

void PostPrintChoicePointNode(const std::string& inNodePath, const bool inIsNodeOpen, HTNDecompositionPrinterContext& ioDecompositionPrinterContext)
{
    HTNDecompositionChoicePointNodeState& ChoicePointNodeState = ioDecompositionPrinterContext.GetChoicePointNodeStateMutable(inNodePath);

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

void PostPrintRegularNode(const std::string& inNodePath, const bool inIsNodeOpen,
                          const HTNNodeSnapshotStepsCollectionDebug& inNodeSnapshotStepsCollection,
                          HTNDecompositionPrinterContext&            ioDecompositionPrinterContext)
{

    HTNDecompositionNodeState& NodeState = ioDecompositionPrinterContext.GetNodeStateMutable(inNodePath);

    const int32 CurrentDecompositionStep = ioDecompositionPrinterContext.GetCurrentDecompositionStep();

    const bool IsCurrentDecompositionStepValid = ioDecompositionPrinterContext.IsCurrentDecompositionStepValid();
    if (IsCurrentDecompositionStepValid)
    {
        const auto NodeSnapshotStepsCollectionIt = inNodeSnapshotStepsCollection.find(CurrentDecompositionStep);
        if (NodeSnapshotStepsCollectionIt != inNodeSnapshotStepsCollection.end())
        {
            const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection   = NodeSnapshotStepsCollectionIt->second;
            const auto                            NodeSnapshotCollectionIt = NodeSnapshotCollection.find(HTNNodeStep::END);
            if (NodeSnapshotCollectionIt != NodeSnapshotCollection.end())
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
                   const HTNNodeSnapshotStepsCollectionDebug& inNodeSnapshotStepsCollection,
                   HTNDecompositionPrinterContext&            ioDecompositionPrinterContext)
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
        PostPrintRegularNode(inNodePath, inIsNodeOpen, inNodeSnapshotStepsCollection, ioDecompositionPrinterContext);
    }
}

bool IsChoicePointNodeValid(const size inDecompositionStep, const int32 inCurrentDecompositionStep, const int32 inMinDecompositionStep,
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

bool IsRegularNodeValid(const size inDecompositionStep, const int32 inCurrentDecompositionStep, const int32 inMinDecompositionStep,
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

bool IsNodeValid(const size inDecompositionStep, const bool inIsChoicePoint, const int32 inCurrentDecompositionStep,
                 const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep)
{
    return (inIsChoicePoint ? IsChoicePointNodeValid(inDecompositionStep, inCurrentDecompositionStep, inMinDecompositionStep, inMaxDecompositionStep)
                            : IsRegularNodeValid(inDecompositionStep, inCurrentDecompositionStep, inMinDecompositionStep, inMaxDecompositionStep));
}
} // namespace

bool HTNDecompositionPrinter::Print(HTNDecompositionPrinterContext& ioDecompositionPrinterContext) const
{
    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDecompositionPrinterContext.GetDomainNode();
    if (!DomainNode)
    {
        HTN_LOG_ERROR("Domain node is null");
        return false;
    }

    const bool Result = GetNodeValue(*DomainNode, ioDecompositionPrinterContext).GetValue<bool>();

    const bool IsSelectedNodeSelected = ioDecompositionPrinterContext.IsSelectedNodeSelected();
    if (!IsSelectedNodeSelected)
    {
        ioDecompositionPrinterContext.UnselectSelectedNode();
    }

    return Result;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::string DomainNodeID                = inDomainNode.GetID();
    HTNPathHandler&   CurrentNodePathHandler      = DecompositionPrinterContext.GetCurrentNodePathHandlerMutable();
    const HTNScope    DomainNodeScope             = HTNScope(DomainNodeID, CurrentNodePathHandler);
    HTNPathHandler&   CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandlerMutable();
    const HTNScope    DomainVariablesScope        = HTNScope(DomainNodeID, CurrentVariablesPathHandler);

    // Top-level compound task node
    const std::string&                               EntryPointID             = DecompositionPrinterContext.GetEntryPointID();
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    return GetNodeValue(*TopLevelCompoundTaskNode, ioContext).GetValue<bool>();
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandlerMutable();
    const HTNScope  AxiomVariablesScope         = HTNScope(inAxiomNode.GetID(), CurrentVariablesPathHandler);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter        = GetNodeValue(*ParameterNode, ioContext);
            const std::string ParameterIDString      = Parameter.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ParameterIDColor = *Parameter.FindListElement(1);
            const ImVec4      ParameterIDImGuiColor =
                ImVec4(ParameterIDColor.FindListElement(0)->GetValue<float>(), ParameterIDColor.FindListElement(1)->GetValue<float>(),
                       ParameterIDColor.FindListElement(2)->GetValue<float>(), ParameterIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());
        }
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
        {
            GetNodeValue(*ConditionNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAxiomNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandlerMutable();
    const HTNScope  MethodVariablesScope        = HTNScope(inMethodNode.GetID(), CurrentVariablesPathHandler);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inMethodNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter        = GetNodeValue(*ParameterNode, ioContext);
            const std::string ParameterIDString      = Parameter.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ParameterIDColor = *Parameter.FindListElement(1);
            const ImVec4      ParameterIDImGuiColor =
                ImVec4(ParameterIDColor.FindListElement(0)->GetValue<float>(), ParameterIDColor.FindListElement(1)->GetValue<float>(),
                       ParameterIDColor.FindListElement(2)->GetValue<float>(), ParameterIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ParameterIDImGuiColor, ParameterIDString.c_str());
        }
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            GetNodeValue(*BranchNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inMethodNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inBranchNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
        {
            GetNodeValue(*PreConditionNode, ioContext);
        }

        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
        for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
        {
            GetNodeValue(*TaskNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_None;
    return PrintNode(inBranchNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inConditionNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument        = GetNodeValue(*ArgumentNode, ioContext);
            const std::string ArgumentIDString      = Argument.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandler();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    const HTNNodeBehaviorFunction       NodeBehaviorFunction;
    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, std::format("#{}", IDString).c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument        = GetNodeValue(*ArgumentNode, ioContext);
            const std::string ArgumentIDString      = Argument.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode      = inAxiomConditionNode.GetIDNode();
        const std::string                                         AxiomNodeID = GetNodeValue(*IDNode, ioContext).GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode>                 AxiomNode   = DomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode, ioContext);
    };

    const HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandler();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNode(inAxiomConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string ID = "and";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, ID.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAndConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string ID = "or";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, ID.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inOrConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string ID = "alt";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, ID.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inAltConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string ID = "not";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, ID.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode, ioContext);
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inNotConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inCompoundTaskNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument        = GetNodeValue(*ArgumentNode, ioContext);
            const std::string ArgumentIDString      = Argument.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode       = inCompoundTaskNode.GetIDNode();
        const std::string                                         MethodNodeID = GetNodeValue(*IDNode, ioContext).GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode>                MethodNode   = DomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode, ioContext);
    };

    const HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandler();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNode(inCompoundTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inPrimitiveTaskNode.GetIDNode();
    static constexpr bool                                     ShouldDoubleQuoteString = false;
    const std::string                                         IDString = GetNodeValue(*IDNode, ioContext).ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument        = GetNodeValue(*ArgumentNode, ioContext);
            const std::string ArgumentIDString      = Argument.FindListElement(0)->ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const HTNPathHandler& CurrentVariablesPathHandler = DecompositionPrinterContext.GetCurrentVariablesPathHandler();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariablesPathHandler, inNodeLabel);
    };

    const HTNNodeBehaviorFunction       NodeBehaviorFunction;
    static constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNode(inPrimitiveTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNIdentifierExpressionNode&      inIdentifierExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     IdentifierString        = inIdentifierExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    return IdentifierString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNLiteralExpressionNode&         inLiteralExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    static constexpr bool   ShouldDoubleQuoteString = true;
    const std::string       LiteralString           = inLiteralExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    static constexpr ImVec4 LiteralColor            = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({LiteralString, HTNAtomList({LiteralColor.x, LiteralColor.y, LiteralColor.z, LiteralColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableExpressionNode&        inVariableExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     VariableString          = std::format("?{}", inVariableExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
    const std::string&    Variable                = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const ImVec4          VariableColor           = HTNImGuiHelpers::GetVariableColor(Variable);
    return HTNAtomList({VariableString, HTNAtomList({VariableColor.x, VariableColor.y, VariableColor.z, VariableColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantExpressionNode&        inConstantExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    static constexpr bool   ShouldDoubleQuoteString = false;
    const std::string       ConstantString          = std::format("@{}", inConstantExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
    static constexpr ImVec4 ConstantColor           = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({ConstantString, HTNAtomList({ConstantColor.x, ConstantColor.y, ConstantColor.z, ConstantColor.w})});
}

bool HTNDecompositionPrinter::PrintNode(const HTNNodeBase& inNode, const HTNNodeTitleFunction inNodeTitleFunction,
                                        const HTNNodeBehaviorFunction inNodeBehaviorFunction, const HTNNodeFunction inNodeFunction,
                                        const ImGuiTreeNodeFlags inTreeNodeFlags, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNPathHandler& CurrentNodePathHandler = DecompositionPrinterContext.GetCurrentNodePathHandlerMutable();
    const HTNScope  NodeScope              = HTNScope(inNode.GetID(), CurrentNodePathHandler);

    const HTNDecompositionSnapshotDebug& DecompositionSnapshot = DecompositionPrinterContext.GetDecompositionSnapshot();
    const std::string&                   CurrentNodePath       = CurrentNodePathHandler.GetPath();
    const HTNNodeSnapshotHistoryDebug*   NodeSnapshotHistory   = DecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!NodeSnapshotHistory)
    {
        return false;
    }

    const bool                                 IsChoicePoint               = NodeSnapshotHistory->IsChoicePoint();
    const HTNNodeSnapshotStepsCollectionDebug& NodeSnapshotStepsCollection = NodeSnapshotHistory->GetNodeSnapshotStepsCollection();

    const auto LastIt                = NodeSnapshotStepsCollection.rbegin();
    const size LastDecompositionStep = LastIt->first;

    PrePrintNode(CurrentNodePath, IsChoicePoint, NodeSnapshotStepsCollection, LastDecompositionStep, DecompositionPrinterContext);

    const HTNNodeStep CurrentNodeStep              = DecompositionPrinterContext.GetNodeStep(CurrentNodePath, IsChoicePoint);
    const int32       CurrentNodeDecompositionStep = DecompositionPrinterContext.GetNodeDecompositionStep(CurrentNodePath, IsChoicePoint);
    const bool        IsCurrentNodeOpen = DecompositionPrinterContext.IsNodeOpen(CurrentNodePath, CurrentNodeDecompositionStep, IsChoicePoint);

    const int32 MinDecompositionStep   = DecompositionPrinterContext.GetMinDecompositionStep();
    const int32 MaxDecompositionStep   = DecompositionPrinterContext.GetMaxDecompositionStep();
    const bool  ShouldIgnoreImGuiState = DecompositionPrinterContext.ShouldIgnoreImGuiState();
    const bool  IsCurrentNodeVisible   = DecompositionPrinterContext.IsCurrentNodeVisible();

    bool NewIsCurrentNodeOpen = IsCurrentNodeOpen;

    for (auto It = NodeSnapshotStepsCollection.begin(); It != NodeSnapshotStepsCollection.end(); ++It)
    {
        const size DecompositionStep = It->first;
        if (!IsNodeValid(DecompositionStep, IsChoicePoint, CurrentNodeDecompositionStep, MinDecompositionStep, MaxDecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection = It->second;
        const HTNNodeSnapshotDebug&           NodeSnapshot           = NodeSnapshotCollection.at(CurrentNodeStep);

        const std::string NodeLabel =
            IsChoicePoint ? std::format("##{}{}", CurrentNodePath, DecompositionStep) : std::format("##{}", CurrentNodePath);
        const HTNDecompositionNode Node          = inNodeFunction ? inNodeFunction(NodeSnapshot, NodeLabel) : HTNDecompositionNode();
        ImGuiTreeNodeFlags         TreeNodeFlags = inTreeNodeFlags | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        const bool IsNodeSelected = DecompositionPrinterContext.IsNodeSelected(NodeLabel);
        if (IsNodeSelected)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;

            DecompositionPrinterContext.SelectNode(Node);
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
                const bool   Result       = NodeSnapshot.GetResult();
                const bool   IsSuccessful = Result && (LastDecompositionStep == DecompositionStep);
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
                    HTNDecompositionWatchTooltipPrinterContext(DomainNode, Node, TooltipMode);
                mDecompositionWatchTooltipPrinter.Print(DecompositionWatchTooltipPrinterContext);
            }

            if (HTNImGuiHelpers::IsCurrentItemSelected())
            {
                DecompositionPrinterContext.SelectNode(Node);
            }

            if (IsChoicePoint)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("%i", DecompositionStep);
            }

            if (inNodeTitleFunction)
            {
                inNodeTitleFunction(NodeSnapshot, CurrentNodeStep);
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
                    ? ((NodeSnapshotStepsCollection.end() != NextIt) ? static_cast<const int32>(NextIt->first) : std::numeric_limits<int32>::max())
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

    PostPrintNode(CurrentNodePath, IsChoicePoint, NewIsCurrentNodeOpen, NodeSnapshotStepsCollection, DecompositionPrinterContext);

    return true;
}
#endif
