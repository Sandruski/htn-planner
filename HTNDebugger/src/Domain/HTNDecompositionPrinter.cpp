#include "Domain/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionWatchTooltipPrinter.h"
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
#include "HTNImGuiHelpers.h"

#include "imgui.h"

// TODO salvarez Do not make this static or reset it when new decomposition
std::unordered_map<std::string, HTNNodeState> HTNDecompositionPrinter::mNodeStates;

bool HTNDecompositionPrinter::Print(HTNDecompositionNode& ioSelectedNode)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    mSelectedNode = ioSelectedNode;

    // TODO salvarez
    // Open successful decomposition by default
    // mCurrentDecompositionStep = static_cast<int>(mDecompositionSnapshot.GetDecompositionStep());
    mCurrentDecompositionStep     = 0;
    mChoicePointDecompositionStep = -1;

    mShouldDisplay            = true;
    mShouldDisplayChoicePoint = true;

    GetNodeValue(*DomainNode).GetValue<bool>();

    ioSelectedNode = mSelectedNode;

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNDomainNode& inDomainNode)
{
    const std::string  DomainNodeID                 = inDomainNode.GetID();
    const HTNNodeScope DomainNodeScope              = HTNNodeScope(mCurrentNodePath, DomainNodeID);
    const HTNNodeScope DomainVariableScopeNodeScope = HTNNodeScope(mCurrentVariableScopeNodePath, DomainNodeID);

    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDecompositionHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    GetNodeValue(*TopLevelCompoundTaskNode);

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNNodeScope AxiomVariableScopeNodeScope = HTNNodeScope(mCurrentVariableScopeNodePath, inAxiomNode.GetID());

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter         = GetNodeValue(*ParameterNode);
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
            GetNodeValue(*ConditionNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAxiomNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodVariableScopeNodeScope = HTNNodeScope(mCurrentVariableScopeNodePath, inMethodNode.GetID());

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inMethodNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeState&         inNodeState) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
        {
            const HTNAtom     Parameter         = GetNodeValue(*ParameterNode);
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
            GetNodeValue(*BranchNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ParameterNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inMethodNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inBranchNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeState&         inNodeState) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
        {
            GetNodeValue(*PreConditionNode);
        }

        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
        for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
        {
            GetNodeValue(*TaskNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_None;
    return PrintNodeSnapshotHistory(inBranchNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode);
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

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep              NodeStep             = HTNNodeStep::END;
    constexpr HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags       TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inConditionNode, NodeStep, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, std::format("#{}", IDString).c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode);
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

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomConditionNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode);

        const std::string                         AxiomNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode> AxiomNode   = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode);
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inAxiomConditionNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const std::string IDString = "and";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAndConditionNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const std::string IDString = "or";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inOrConditionNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const std::string IDString = "alt";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAltConditionNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const std::string IDString = "not";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeState& inNodeState) {
        const ImVec4 IDColor = HTNImGuiHelpers::GetNodeColor(inNodeSnapshot, inNodeState);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode);
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inNotConditionNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inCompoundTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeState&         inNodeState) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode);
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

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode);

        const std::string                          MethodNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode> MethodNode   = mDomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode);
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep        NodeStep      = HTNNodeStep::START;
    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inCompoundTaskNode, NodeStep, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inPrimitiveTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                       MAYBE_UNUSED const HTNNodeState&         inNodeState) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
        {
            const HTNAtom     Argument         = GetNodeValue(*ArgumentNode);
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

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    constexpr HTNNodeStep              NodeStep             = HTNNodeStep::START;
    constexpr HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags       TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeStep, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode)
{
    const HTNAtom&     Identifier              = inIdentifierExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = false;
    const std::string& IdentifierString        = Identifier.ToString(ShouldDoubleQuoteString);
    return IdentifierString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    const HTNAtom&     Literal                 = inLiteralExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = true;
    const std::string& LiteralString           = Literal.ToString(ShouldDoubleQuoteString);
    constexpr ImVec4   LiteralColor            = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({LiteralString, HTNAtomList({LiteralColor.x, LiteralColor.y, LiteralColor.z, LiteralColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    const std::string VariableString = std::format("?{}", inVariableExpressionNode.ToString());
    const ImVec4      VariableColor  = HTNImGuiHelpers::GetVariableColor(VariableString);
    return HTNAtomList({VariableString, HTNAtomList({VariableColor.x, VariableColor.y, VariableColor.z, VariableColor.w})});
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    const std::string ConstantString = std::format("@{}", inConstantExpressionNode.ToString());
    constexpr ImVec4  ConstantColor  = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({ConstantString, HTNAtomList({ConstantColor.x, ConstantColor.y, ConstantColor.z, ConstantColor.w})});
}

bool HTNDecompositionPrinter::PrintNodeSnapshotHistory(const HTNNodeBase& inNode, MAYBE_UNUSED const HTNNodeStep inNodeStep,
                                                       const HTNNodeTitleFunction&    inNodeTitleFunction,
                                                       const HTNNodeBehaviorFunction* inNodeBehaviorFunction, const HTNNodeFunction& inNodeFunction,
                                                       const ImGuiTreeNodeFlags inTreeNodeFlags)
{
    const HTNNodeScope NodeScope = HTNNodeScope(mCurrentNodePath, inNode.GetID());

    const std::string&                 CurrentNodePath     = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* NodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!NodeSnapshotHistory)
    {
        return false;
    }

    /*
    if (mCurrentDecompositionStep == -1)
    {
        return false;
    }
    */

    const HTNNodeSnapshotStepsCollectionDebug& NodeSnapshotStepsCollection = NodeSnapshotHistory->GetNodeSnapshotStepsCollection();
    const bool                                 IsChoicePoint               = NodeSnapshotHistory->IsChoicePoint();

    const auto        LastIt                = NodeSnapshotStepsCollection.rbegin();
    const std::size_t LastDecompositionStep = LastIt->first;

    // Get node(s)
    const auto CurrentNodeStateIt = mNodeStates.find(CurrentNodePath);
    if (CurrentNodeStateIt != mNodeStates.end())
    {
        HTNNodeState& CurrentNodeState = CurrentNodeStateIt->second;
        mCurrentNodeStep               = CurrentNodeState.GetNodeStep();

        const HTNNodeDirection NodeDirection = CurrentNodeState.GetNodeDirection();
        if (NodeDirection == HTNNodeDirection::BOTTOM_UP)
        {
            mCurrentDecompositionStep = CurrentNodeState.GetDecompositionStep();
        }
    }
    else
    {
        if (IsChoicePoint)
        {
            mCurrentDecompositionStep    = -1;
            mCurrentNodeStep             = HTNNodeStep::END;
            mNodeStates[CurrentNodePath] = HTNNodeState(mCurrentDecompositionStep, mCurrentNodeStep, HTNNodeDirection::BOTTOM_UP);
        }
        else
        {
            mCurrentNodeStep             = HTNNodeStep::START;
            mNodeStates[CurrentNodePath] = HTNNodeState(mCurrentDecompositionStep, mCurrentNodeStep, HTNNodeDirection::TOP_DOWN);
        }
    }

    // Print node(s)
    for (auto It = NodeSnapshotStepsCollection.begin(); It != NodeSnapshotStepsCollection.end(); ++It)
    {
        const std::size_t                     DecompositionStep      = It->first;
        const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection = It->second;

        if (-1 == mCurrentDecompositionStep)
        {
            if (!IsChoicePoint)
            {
                // Print all choice points
                continue; // TODO salvarez return
            }
        }
        else if (DecompositionStep != mCurrentDecompositionStep)
        {
            // Print node (choice point or not)
            continue; // TODO salvarez return
        }

        // TODO salvarez Fix hack
        // TODO We miss information on upper nodes (they have start but not end)
        auto NodeStepIt = NodeSnapshotCollection.find(mCurrentNodeStep);
        if (NodeStepIt == NodeSnapshotCollection.end())
        {
            if (HTNNodeStep::START == mCurrentNodeStep)
            {
                NodeStepIt = NodeSnapshotCollection.find(HTNNodeStep::END);
                if (NodeStepIt == NodeSnapshotCollection.end())
                {
                    continue;
                }
            }
            else if (HTNNodeStep::END == mCurrentNodeStep)
            {
                NodeStepIt = NodeSnapshotCollection.find(HTNNodeStep::START);
                if (NodeStepIt == NodeSnapshotCollection.end())
                {
                    continue;
                }
            }
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = NodeStepIt->second;

        /*
        const bool IsDefaultOpen = (DecompositionStep == mCurrentDecompositionStep);
        if (mShouldResetView)
        {
            HTNImGuiHelpers::SetTreeNodeOpen(Label, IsDefaultOpen);
        }

        if (IsDefaultOpen)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        */

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | inTreeNodeFlags;
        if (!IsChoicePoint)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        if (IsChoicePoint)
        {
            // Display arrow
            TreeNodeFlags &= ~ImGuiTreeNodeFlags_Leaf;

            // Push arrow color
            const bool   IsSuccessful = (LastDecompositionStep == DecompositionStep);
            const ImVec4 ArrowColor   = IsSuccessful ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
            ImGui::PushStyleColor(ImGuiCol_Text, ArrowColor);
        }

        /*
        if (mCurrentDecompositionStep == DecompositionStep)
        {
            if (mShouldResetView)
            {
                ImGui::SetNextItemOpen(true);
            }
        }
        */

        const std::string Label  = IsChoicePoint ? std::format("##{}{}", CurrentNodePath, DecompositionStep) : std::format("##{}", CurrentNodePath);
        const bool        IsOpen = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (IsChoicePoint)
        {
            // Pop arrow color
            ImGui::PopStyleColor(1);

            // Set choice point
            // HTNNodeState& CurrentNodeState = mNodeStates[CurrentNodePath];
            // mCurrentDecompositionStep      = IsOpen ? static_cast<int>(DecompositionStep) : -1;
            // CurrentNodeState.SetDecompositionStep(mCurrentDecompositionStep);
            mChoicePointDecompositionStep = IsOpen ? static_cast<int>(DecompositionStep) : -1;
        }

        const HTNDecompositionNode Node = inNodeFunction(NodeSnapshot);

        if (HTNImGuiHelpers::IsCurrentItemHovered())
        {
            HTNDecompositionWatchTooltipPrinter DecompositionWatchTooltipPrinter = HTNDecompositionWatchTooltipPrinter(mDomainNode, Node);
            DecompositionWatchTooltipPrinter.Print(mShouldPrintFullTooltip);
        }

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(Node);
        }

        // TODO salvarez
        // if (NodeSnapshotCollectionSize > 1)
        //{
        // if (-1 == mCurrentDecompositionStep)
        //{
        ImGui::SameLine();
        ImGui::TextDisabled("%i", DecompositionStep);
        //}
        //}

        inNodeTitleFunction(NodeSnapshot, mNodeStates[CurrentNodePath]);

        if (!IsOpen)
        {
            continue;
        }

        if (inNodeBehaviorFunction)
        {
            (*inNodeBehaviorFunction)();
        }

        if (!(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            ImGui::TreePop();
        }

        break;
    }

    if (IsChoicePoint)
    {
        HTNNodeState& CurrentNodeState = mNodeStates[CurrentNodePath];
        if (-1 == mChoicePointDecompositionStep)
        {
            CurrentNodeState.SetDecompositionStep(-1);
        }
        else
        {
            CurrentNodeState.SetDecompositionStep(mChoicePointDecompositionStep);
        }
    }

    // Set node (not choice point)
    else
    {
        HTNNodeState& CurrentNodeState = mNodeStates[CurrentNodePath];
        if (-1 == mChoicePointDecompositionStep)
        {
            CurrentNodeState.SetNodeStep(HTNNodeStep::START);
            CurrentNodeState.SetNodeDirection(HTNNodeDirection::TOP_DOWN);
        }
        else
        {
            CurrentNodeState.SetDecompositionStep(mChoicePointDecompositionStep);
            CurrentNodeState.SetNodeStep(HTNNodeStep::END);
            CurrentNodeState.SetNodeDirection(HTNNodeDirection::BOTTOM_UP);
        }
    }

    return true;
}
#endif

/*
if (IsChoicePoint)
{
    const bool IsCurrentDecompositionStepSuccessful = (LastDecompositionStep == mCurrentDecompositionStep);
    mShouldDisplayChoicePoint &= IsCurrentDecompositionStepSuccessful;
    const bool HasPendingChoicePoint = (-1 == mCurrentDecompositionStep);
    mShouldDisplayChoicePoint &= !HasPendingChoicePoint;
    mShouldDisplay &= !HasPendingChoicePoint;
}
*/