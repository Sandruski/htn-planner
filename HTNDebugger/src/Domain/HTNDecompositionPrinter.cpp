#include "Domain/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionPrinterContext.h"
#include "Domain/HTNDecompositionWatchTooltipPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNNodeScope.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
HTNDecompositionPrinterContext& GetDecompositionPrinterContext(HTNNodeVisitorContextBase& ioContext)
{
    return static_cast<HTNDecompositionPrinterContext&>(ioContext);
}

bool IsNodeValid(const size inDecompositionStep, const bool inIsChoicePoint, const int32 inCurrentDecompositionStep,
                 const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep)
{
    // Filter available nodes within range [min, max)
    if (!HTNDecompositionHelpers::IsDecompositionStepInRange(static_cast<const int32>(inDecompositionStep), inMinDecompositionStep,
                                                             inMaxDecompositionStep))
    {
        return false;
    }
    else
    {
        // Print all choice points
        if (!HTNDecompositionHelpers::IsDecompositionStepValid(inCurrentDecompositionStep))
        {
            if (!inIsChoicePoint)
            {
                return false;
            }
        }
        else if (static_cast<const int32>(inDecompositionStep) != inCurrentDecompositionStep) // Print node (choice point or not)
        {
            return false;
        }
    }

    return true;
}
} // namespace

bool HTNDecompositionPrinter::Print(HTNDecompositionPrinterContext& ioDecompositionPrinterContext) const
{
    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDecompositionPrinterContext.GetDomainNode();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    GetNodeValue(*DomainNode, ioDecompositionPrinterContext).GetValue<bool>();

    const bool IsSelectedNodeSelected = ioDecompositionPrinterContext.IsSelectedNodeSelected();
    if (!IsSelectedNodeSelected)
    {
        ioDecompositionPrinterContext.UnselectSelectedNode();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::string  DomainNodeID                 = inDomainNode.GetID();
    HTNNodePath&       CurrentNodePath              = DecompositionPrinterContext.GetCurrentNodePathMutable();
    const HTNNodeScope DomainNodeScope              = HTNNodeScope(DomainNodeID, CurrentNodePath);
    HTNNodePath&       CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePathMutable();
    const HTNNodeScope DomainVariableScopeNodeScope = HTNNodeScope(DomainNodeID, CurrentVariableScopeNodePath);

    // Top-level compound task node
    const std::string&                               EntryPointID             = DecompositionPrinterContext.GetEntryPointID();
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(EntryPointID);
    GetNodeValue(*TopLevelCompoundTaskNode, ioContext);

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNNodePath&       CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePathMutable();
    const HTNNodeScope AxiomVariableScopeNodeScope  = HTNNodeScope(inAxiomNode.GetID(), CurrentVariableScopeNodePath);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter         = GetNodeValue(*ParameterNode, ioContext);
            const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
            const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ParameterIDColor  = *Parameter.FindListElement(1);
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
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAxiomNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNNodePath&       CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePathMutable();
    const HTNNodeScope MethodVariableScopeNodeScope = HTNNodeScope(inMethodNode.GetID(), CurrentVariableScopeNodePath);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inMethodNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter         = GetNodeValue(*ParameterNode, ioContext);
            const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
            const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ParameterIDColor  = *Parameter.FindListElement(1);
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
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inMethodNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inBranchNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_None;
    return PrintNodeSnapshotHistory(inBranchNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode, ioContext);
            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const HTNNodePath& CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePath();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction;
    constexpr ImGuiTreeNodeFlags  TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, std::format("#{}", IDString).c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode, ioContext);
            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomConditionNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode, ioContext);

        const std::string                         AxiomNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode> AxiomNode   = DomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode, ioContext);
    };

    const HTNNodePath& CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePath();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inAxiomConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string IDString = "and";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAndConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string IDString = "or";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inOrConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string IDString = "alt";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAltConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) const
{
    const std::string IDString = "not";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeStep);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode, ioContext);
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, inNodeLabel);
    };

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inNotConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inCompoundTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode, ioContext);
            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const std::shared_ptr<const HTNDomainNode>& DomainNode = DecompositionPrinterContext.GetDomainNode();

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode, ioContext);

        const std::string                          MethodNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode> MethodNode   = DomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode, ioContext);
    };

    const HTNNodePath& CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePath();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inCompoundTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inPrimitiveTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode, ioContext);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeStep           inNodeStep) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode, ioContext);
            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const HTNAtom&    ArgumentIDColor  = *Argument.FindListElement(1);
            const ImVec4      ArgumentIDImGuiColor =
                ImVec4(ArgumentIDColor.FindListElement(0)->GetValue<float>(), ArgumentIDColor.FindListElement(1)->GetValue<float>(),
                       ArgumentIDColor.FindListElement(2)->GetValue<float>(), ArgumentIDColor.FindListElement(3)->GetValue<float>());
            ImGui::SameLine();
            ImGui::TextColored(ArgumentIDImGuiColor, ArgumentIDString.c_str());
        }
    };

    const HTNNodePath& CurrentVariableScopeNodePath = DecompositionPrinterContext.GetCurrentVariableScopeNodePath();

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, CurrentVariableScopeNodePath, inNodeLabel);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction;
    constexpr ImGuiTreeNodeFlags  TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags, ioContext);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNIdentifierExpressionNode&      inIdentifierExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    const HTNAtom&     Identifier              = inIdentifierExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = false;
    const std::string& IdentifierString        = Identifier.ToString(ShouldDoubleQuoteString);
    return IdentifierString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNLiteralExpressionNode&         inLiteralExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    const HTNAtom&     Literal                 = inLiteralExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = true;
    const std::string& LiteralString           = Literal.ToString(ShouldDoubleQuoteString);
    constexpr ImVec4   LiteralColor            = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({LiteralString, HTNAtomList({LiteralColor.x, LiteralColor.y, LiteralColor.z, LiteralColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableExpressionNode&        inVariableExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    const std::string VariableString = std::format("?{}", inVariableExpressionNode.ToString());
    const ImVec4      VariableColor  = HTNImGuiHelpers::GetVariableColor(VariableString);
    return HTNAtomList({VariableString, HTNAtomList({VariableColor.x, VariableColor.y, VariableColor.z, VariableColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantExpressionNode&        inConstantExpressionNode,
                                       MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    const std::string ConstantString = std::format("@{}", inConstantExpressionNode.ToString());
    constexpr ImVec4  ConstantColor  = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({ConstantString, HTNAtomList({ConstantColor.x, ConstantColor.y, ConstantColor.z, ConstantColor.w})});
}

bool HTNDecompositionPrinter::PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const HTNNodeTitleFunction inNodeTitleFunction,
                                                       const HTNNodeBehaviorFunction inNodeBehaviorFunction, const HTNNodeFunction inNodeFunction,
                                                       const ImGuiTreeNodeFlags inTreeNodeFlags, HTNNodeVisitorContextBase& ioContext) const
{
    HTNDecompositionPrinterContext& DecompositionPrinterContext = GetDecompositionPrinterContext(ioContext);

    HTNNodePath&       CurrentNodePath = DecompositionPrinterContext.GetCurrentNodePathMutable();
    const HTNNodeScope NodeScope       = HTNNodeScope(inNode.GetID(), CurrentNodePath);

    const std::string&                   CurrentNodePathString = CurrentNodePath.GetNodePath();
    const HTNDecompositionSnapshotDebug& DecompositionSnapshot = DecompositionPrinterContext.GetDecompositionSnapshot();
    const HTNNodeSnapshotHistoryDebug*   NodeSnapshotHistory   = DecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePathString);
    if (!NodeSnapshotHistory)
    {
        return false;
    }

    if (!DecompositionPrinterContext.IsCurrentDecompositionStepValid())
    {
        // Exit if choice point not selected
        return false;
    }

    const HTNNodeSnapshotStepsCollectionDebug& NodeSnapshotStepsCollection = NodeSnapshotHistory->GetNodeSnapshotStepsCollection();
    const bool                                 IsChoicePoint               = NodeSnapshotHistory->IsChoicePoint();

    const auto        LastIt                = NodeSnapshotStepsCollection.rbegin();
    const size LastDecompositionStep = LastIt->first;

    // Initialize node(s)
    if (IsChoicePoint)
    {
        HTNDecompositionChoicePointNodeState* CurrentChoicePointNodeState =
            DecompositionPrinterContext.FindChoicePointNodeStateMutable(CurrentNodePathString);
        if (CurrentChoicePointNodeState)
        {
            const bool ShouldRefreshNodeStates = DecompositionPrinterContext.ShouldRefreshNodeStates();
            if (ShouldRefreshNodeStates)
            {
                const int32 MinDecompositionStep = DecompositionPrinterContext.GetMinDecompositionStep();
                const int32 MaxDecompositionStep = DecompositionPrinterContext.GetMaxDecompositionStep();
                const int32 DecompositionStep =
                    CurrentChoicePointNodeState->FindOpenDecompositionStepInRange(MinDecompositionStep, MaxDecompositionStep);
                CurrentChoicePointNodeState->SetDecompositionStep(DecompositionStep);
            }
        }
        else
        {
            // Default open successful decomposition step
            const int32                                  DecompositionStep    = static_cast<const int32>(LastDecompositionStep);
            constexpr bool                             IsOpen               = true;
            const HTNDecompositionChoicePointNodeState ChoicePointNodeState = HTNDecompositionChoicePointNodeState(DecompositionStep, IsOpen);
            DecompositionPrinterContext.AddChoicePointNodeState(CurrentNodePathString, ChoicePointNodeState);
            DecompositionPrinterContext.RefreshNodeStates();
        }
    }
    else
    {
        HTNDecompositionNodeState* CurrentNodeState = DecompositionPrinterContext.FindNodeStateMutable(CurrentNodePathString);
        if (CurrentNodeState)
        {
            const bool ShouldRefreshNodeStates = DecompositionPrinterContext.ShouldRefreshNodeStates();
            if (ShouldRefreshNodeStates)
            {
                const int32 CurrentDecompositionStep = DecompositionPrinterContext.GetCurrentDecompositionStep();
                if (DecompositionPrinterContext.IsCurrentDecompositionStepValid())
                {
                    const auto NodeSnapshotStepsCollectionIt = NodeSnapshotStepsCollection.find(CurrentDecompositionStep);
                    if (NodeSnapshotStepsCollectionIt != NodeSnapshotStepsCollection.end())
                    {
                        const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection   = NodeSnapshotStepsCollectionIt->second;
                        const auto                            NodeSnapshotCollectionIt = NodeSnapshotCollection.find(HTNNodeStep::END);
                        if (NodeSnapshotCollectionIt != NodeSnapshotCollection.end())
                        {
                            CurrentNodeState->SetDecompositionStep(CurrentDecompositionStep);
                        }
                    }
                }
                else
                {
                    CurrentNodeState->SetDecompositionStep(CurrentDecompositionStep);
                }
            }
        }
        else
        {
            // Default open current decomposition step
            constexpr int32                   DecompositionStep = HTNDecompositionHelpers::kInvalidDecompositionStep;
            constexpr bool                  IsOpen            = true;
            const HTNDecompositionNodeState NodeState         = HTNDecompositionNodeState(DecompositionStep, IsOpen);
            DecompositionPrinterContext.AddNodeState(CurrentNodePathString, NodeState);
            DecompositionPrinterContext.RefreshNodeStates();
        }
    }

    // Get node(s)
    const HTNNodeStep CurrentNodeStep              = DecompositionPrinterContext.GetNodeStep(CurrentNodePathString, IsChoicePoint);
    const int32         CurrentNodeDecompositionStep = DecompositionPrinterContext.GetNodeDecompositionStep(CurrentNodePathString, IsChoicePoint);
    bool              IsCurrentNodeOpen = DecompositionPrinterContext.IsNodeOpen(CurrentNodePathString, CurrentNodeDecompositionStep, IsChoicePoint);

    // Print node(s)
    const int32  MinDecompositionStep   = DecompositionPrinterContext.GetMinDecompositionStep();
    const int32  MaxDecompositionStep   = DecompositionPrinterContext.GetMaxDecompositionStep();
    const bool ShouldIgnoreImGuiState = DecompositionPrinterContext.ShouldIgnoreImGuiState();
    const bool IsCurrentNodeVisible   = DecompositionPrinterContext.IsCurrentNodeVisible();

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
            IsChoicePoint ? std::format("##{}{}", CurrentNodePathString, DecompositionStep) : std::format("##{}", CurrentNodePathString);
        const HTNDecompositionNode Node          = inNodeFunction ? inNodeFunction(NodeSnapshot, NodeLabel) : HTNDecompositionNode();
        ImGuiTreeNodeFlags         TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | inTreeNodeFlags;

        const bool IsNodeSelected = DecompositionPrinterContext.IsNodeSelected(NodeLabel);
        if (IsNodeSelected)
        {
            DecompositionPrinterContext.SelectNode(Node);
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
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
                IsCurrentNodeOpen =
                    DecompositionPrinterContext.IsNodeOpen(CurrentNodePathString, static_cast<const int32>(DecompositionStep), IsChoicePoint);

                // Push arrow color
                const bool   IsSuccessful = (LastDecompositionStep == DecompositionStep);
                const ImVec4 ArrowColor   = IsSuccessful ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
                ImGui::PushStyleColor(ImGuiCol_Text, ArrowColor);
            }

            HTNImGuiHelpers::SetTreeNodeOpen(NodeLabel, IsCurrentNodeOpen);

            const bool WasCurrentNodeOpen = IsCurrentNodeOpen;
            IsOpen                        = ImGui::TreeNodeEx(NodeLabel.c_str(), TreeNodeFlags);

            if (!ShouldIgnoreImGuiState)
            {
                IsCurrentNodeOpen = IsOpen;
            }

            if (WasCurrentNodeOpen != IsCurrentNodeOpen)
            {
                DecompositionPrinterContext.RefreshNodeStates();
            }

            if (IsChoicePoint)
            {
                // Pop arrow color
                ImGui::PopStyleColor(1);
            }

            if (HTNImGuiHelpers::IsCurrentItemHovered())
            {
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
                IsCurrentNodeOpen ? static_cast<const int32>(DecompositionStep) : HTNDecompositionHelpers::kInvalidDecompositionStep;
            DecompositionPrinterContext.SetCurrentDecompositionStep(NewCurrentDecompositionStep);

            // Set range to filter next nodes
            const int32 NewMinDecompositionStep = IsCurrentNodeOpen ? NewCurrentDecompositionStep : std::numeric_limits<int32>::max();
            DecompositionPrinterContext.SetMinDecompositionStep(NewMinDecompositionStep);

            auto NextIt = It;
            ++NextIt;
            const int32 NewMaxDecompositionStep =
                IsCurrentNodeOpen
                    ? ((NodeSnapshotStepsCollection.end() != NextIt) ? static_cast<const int32>(NextIt->first) : std::numeric_limits<int32>::max())
                    : std::numeric_limits<int32>::min();
            DecompositionPrinterContext.SetMaxDecompositionStep(NewMaxDecompositionStep);
        }

        // Grouping point determines visibility
        const bool IsGroupingPoint = !(TreeNodeFlags & ImGuiTreeNodeFlags_Leaf);
        if (IsGroupingPoint)
        {
            if (!IsCurrentNodeOpen)
            {
                // TODO salvarez Split this function into before and after and create temp variables for values like false
                DecompositionPrinterContext.SetIsCurrentNodeVisible(false);
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

        if (IsCurrentNodeOpen)
        {
            break;
        }
    }

    // Update node(s)
    const bool ShouldRefreshNodeStates = DecompositionPrinterContext.ShouldRefreshNodeStates();
    if (ShouldRefreshNodeStates)
    {
        const int32 CurrentDecompositionStep = DecompositionPrinterContext.GetCurrentDecompositionStep();
        if (IsChoicePoint)
        {
            HTNDecompositionChoicePointNodeState* CurrentNodeState =
                DecompositionPrinterContext.FindChoicePointNodeStateMutable(CurrentNodePathString);
            if (CurrentNodeState)
            {
                CurrentNodeState->SetDecompositionStep(CurrentDecompositionStep);

                if (DecompositionPrinterContext.IsCurrentDecompositionStepValid())
                {
                    CurrentNodeState->SetIsOpen(CurrentDecompositionStep, IsCurrentNodeOpen);
                }
                else
                {
                    CurrentNodeState->SetIsOpen(CurrentNodeDecompositionStep, IsCurrentNodeOpen);
                }
            }
        }
        else
        {
            HTNDecompositionNodeState* CurrentNodeState = DecompositionPrinterContext.FindNodeStateMutable(CurrentNodePathString);
            if (CurrentNodeState)
            {
                if (DecompositionPrinterContext.IsCurrentDecompositionStepValid())
                {
                    const auto NodeSnapshotStepsCollectionIt = NodeSnapshotStepsCollection.find(CurrentDecompositionStep);
                    if (NodeSnapshotStepsCollectionIt != NodeSnapshotStepsCollection.end())
                    {
                        const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection   = NodeSnapshotStepsCollectionIt->second;
                        const auto                            NodeSnapshotCollectionIt = NodeSnapshotCollection.find(HTNNodeStep::END);
                        if (NodeSnapshotCollectionIt != NodeSnapshotCollection.end())
                        {
                            CurrentNodeState->SetDecompositionStep(CurrentDecompositionStep);
                        }
                    }
                }
                else
                {
                    CurrentNodeState->SetDecompositionStep(CurrentDecompositionStep);
                }

                CurrentNodeState->SetIsOpen(IsCurrentNodeOpen);
            }
        }
    }

    return true;
}
#endif
