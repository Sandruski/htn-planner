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
std::string                                   HTNDecompositionPrinter::mCurrentSelectedNodeLabel;

bool HTNDecompositionPrinter::Print(HTNDecompositionNode& ioSelectedNode)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    mCurrentSelectedNode = ioSelectedNode;

    GetNodeValue(*DomainNode).GetValue<bool>();

    if (!mIsCurrentSelectedNodeSelected)
    {
        mCurrentSelectedNode = HTNDecompositionNode();
    }

    ioSelectedNode = mCurrentSelectedNode;

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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAxiomNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inMethodNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_None;
    return PrintNodeSnapshotHistory(inBranchNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags       TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inAxiomConditionNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAndConditionNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inOrConditionNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inAltConditionNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inNotConditionNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen;
    return PrintNodeSnapshotHistory(inCompoundTaskNode, NodeTitleFunction, &NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

    constexpr HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags       TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
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

bool HTNDecompositionPrinter::PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const HTNNodeTitleFunction& inNodeTitleFunction,
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

    const HTNNodeSnapshotStepsCollectionDebug& NodeSnapshotStepsCollection = NodeSnapshotHistory->GetNodeSnapshotStepsCollection();
    const bool                                 IsChoicePoint               = NodeSnapshotHistory->IsChoicePoint();

    const auto        LastIt                = NodeSnapshotStepsCollection.rbegin();
    const std::size_t LastDecompositionStep = LastIt->first;

    // Get node(s)
    HTNNodeStep CurrentNodeStep   = HTNNodeStep::NONE;
    bool        IsCurrentNodeOpen = false;

    const auto CurrentNodeStateIt = mNodeStates.find(CurrentNodePath);
    if (CurrentNodeStateIt != mNodeStates.end())
    {
        HTNNodeState& CurrentNodeState = CurrentNodeStateIt->second;

        if (kInvalidDecompositionStep == mCurrentDecompositionStep)
        {
            // End choice point
            if (IsChoicePoint)
            {
                CurrentNodeState.SetDecompositionStep(mCurrentDecompositionStep);
                CurrentNodeState.SetIsOpen(IsCurrentNodeOpen);
            }

            return true;
        }

        // Reset node
        if (mShouldResetView)
        {
            // Default open successful choice point
            if (IsChoicePoint)
            {
                mCurrentDecompositionStep = static_cast<int>(LastDecompositionStep);
            }

            CurrentNodeStep                      = IsChoicePoint ? HTNNodeStep::END : HTNNodeStep::START;
            const HTNNodeDirection NodeDirection = IsChoicePoint ? HTNNodeDirection::BOTTOM_UP : HTNNodeDirection::TOP_DOWN;
            IsCurrentNodeOpen                    = true;

            CurrentNodeState.SetDecompositionStep(mCurrentDecompositionStep);
            CurrentNodeState.SetNodeStep(CurrentNodeStep);
            CurrentNodeState.SetNodeDirection(NodeDirection);
            CurrentNodeState.SetIsOpen(IsCurrentNodeOpen);

            mShouldUpdateNodeStates = true;
        }
        else
        {
            const HTNNodeDirection NodeDirection = CurrentNodeState.GetNodeDirection();
            if (NodeDirection == HTNNodeDirection::BOTTOM_UP)
            {
                mCurrentDecompositionStep = CurrentNodeState.GetDecompositionStep();
            }

            CurrentNodeStep   = CurrentNodeState.GetNodeStep();
            IsCurrentNodeOpen = CurrentNodeState.IsOpen();
        }
    }
    else // Initialize node
    {
        if (kInvalidDecompositionStep == mCurrentDecompositionStep)
        {
            return true;
        }

        // Default open successful choice point
        if (IsChoicePoint)
        {
            mCurrentDecompositionStep = static_cast<int>(LastDecompositionStep);
        }

        CurrentNodeStep                      = IsChoicePoint ? HTNNodeStep::END : HTNNodeStep::START;
        const HTNNodeDirection NodeDirection = IsChoicePoint ? HTNNodeDirection::BOTTOM_UP : HTNNodeDirection::TOP_DOWN;
        IsCurrentNodeOpen                    = true;
        mNodeStates[CurrentNodePath]         = HTNNodeState(mCurrentDecompositionStep, CurrentNodeStep, NodeDirection, IsCurrentNodeOpen);

        mShouldUpdateNodeStates = true;
    }

    // Print node(s)
    bool IsOpen = false;

    auto It = NodeSnapshotStepsCollection.begin();
    for (; It != NodeSnapshotStepsCollection.end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsValidNode(DecompositionStep, IsChoicePoint))
        {
            continue;
        }

        const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection = It->second;
        const HTNNodeSnapshotDebug&           NodeSnapshot           = NodeSnapshotCollection.at(CurrentNodeStep);

        const HTNDecompositionNode Node = inNodeFunction(NodeSnapshot);

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | inTreeNodeFlags;

        const std::string Label = IsChoicePoint ? std::format("##{}{}", CurrentNodePath, DecompositionStep) : std::format("##{}", CurrentNodePath);
        if (IsNodeSelected(Label))
        {
            RefreshSelectedNode(Node);
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

        HTNImGuiHelpers::SetTreeNodeOpen(Label, IsCurrentNodeOpen);
        IsOpen = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (IsChoicePoint)
        {
            // Pop arrow color
            ImGui::PopStyleColor(1);
        }

        if (HTNImGuiHelpers::IsCurrentItemHovered())
        {
            HTNDecompositionWatchTooltipPrinter DecompositionWatchTooltipPrinter = HTNDecompositionWatchTooltipPrinter(mDomainNode, Node);
            DecompositionWatchTooltipPrinter.Print(mShouldPrintFullTooltip);
        }

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(Label, Node);
        }

        // if (IsChoicePoint)
        //{
        ImGui::SameLine();
        ImGui::TextDisabled("%i", DecompositionStep);
        //}

        inNodeTitleFunction(NodeSnapshot, mNodeStates[CurrentNodePath]);

        if (!IsOpen)
        {
            continue;
        }

        // Perform selected node behavior
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

    // Set node(s)
    HTNNodeState& CurrentNodeState = mNodeStates[CurrentNodePath];
    CurrentNodeState.SetIsOpen(IsOpen);

    if (IsChoicePoint)
    {
        if (IsCurrentNodeOpen != IsOpen)
        {
            mShouldUpdateNodeStates = true;
        }

        if (IsOpen)
        {
            mCurrentDecompositionStep            = static_cast<int>(It->first);
            mCurrentChoicePointDecompositionStep = mCurrentDecompositionStep;

            // Set range to filter next nodes
            mMinDecompositionStep = mCurrentDecompositionStep;

            auto NextIt = It;
            ++NextIt;
            if (NextIt != NodeSnapshotStepsCollection.end())
            {
                mMaxDecompositionStep = static_cast<int>(NextIt->first);
            }
        }
        else
        {
            mCurrentDecompositionStep            = kInvalidDecompositionStep;
            mCurrentChoicePointDecompositionStep = kInvalidDecompositionStep;
        }

        if (!mShouldUpdateNodeStates)
        {
            return true;
        }

        CurrentNodeState.SetDecompositionStep(mCurrentChoicePointDecompositionStep);
    }
    else
    {
        if (!mShouldUpdateNodeStates)
        {
            return true;
        }

        if (kInvalidDecompositionStep == mCurrentChoicePointDecompositionStep)
        {
            CurrentNodeState.SetNodeStep(HTNNodeStep::START);
            CurrentNodeState.SetNodeDirection(HTNNodeDirection::TOP_DOWN);
        }
        else
        {
            const auto NodeSnapshotStepsCollectionIt = NodeSnapshotStepsCollection.find(mCurrentChoicePointDecompositionStep);
            if (NodeSnapshotStepsCollectionIt != NodeSnapshotStepsCollection.end())
            {
                const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection   = NodeSnapshotStepsCollectionIt->second;
                const auto                            NodeSnapshotCollectionIt = NodeSnapshotCollection.find(HTNNodeStep::END);
                if (NodeSnapshotCollectionIt != NodeSnapshotCollection.end())
                {
                    CurrentNodeState.SetDecompositionStep(mCurrentChoicePointDecompositionStep);
                    CurrentNodeState.SetNodeStep(HTNNodeStep::END);
                    CurrentNodeState.SetNodeDirection(HTNNodeDirection::BOTTOM_UP);
                }
            }
        }
    }

    return true;
}

bool HTNDecompositionPrinter::IsValidNode(const std::size_t inDecompositionStep, const bool inIsChoicePoint)
{
    // Filter available nodes within range [min, max)
    if (static_cast<int>(inDecompositionStep) < mMinDecompositionStep)
    {
        return false;
    }
    else if (static_cast<int>(inDecompositionStep) >= mMaxDecompositionStep)
    {
        return false;
    }
    else
    {
        // Print all choice points
        if (kInvalidDecompositionStep == mCurrentDecompositionStep)
        {
            if (!inIsChoicePoint)
            {
                return false;
            }
        }
        else if (static_cast<int>(inDecompositionStep) != mCurrentDecompositionStep) // Print node (choice point or not)
        {
            return false;
        }
    }

    return true;
}
#endif
