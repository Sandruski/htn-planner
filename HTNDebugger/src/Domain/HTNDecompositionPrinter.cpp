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

bool HTNDecompositionPrinter::Print(HTNDecompositionNode& ioSelectedNode)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    mSelectedNode = ioSelectedNode;

    // Default open to successful decomposition
    mCurrentDecompositionStep = static_cast<int>(mDecompositionSnapshot.GetDecompositionStep());

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
    std::vector<std::string>                                             ParameterStrings;
    ParameterStrings.reserve(ParameterNodes.size());
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        const HTNAtom     Parameter       = GetNodeValue(*ParameterNode);
        const std::string ParameterString = Parameter.ToString(ShouldDoubleQuoteString);
        ParameterStrings.emplace_back(ParameterString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::string& ParameterString : ParameterStrings)
        {
            constexpr ImVec4 ParameterColor = HTNImGuiHelpers::kParameterColor;
            ImGui::SameLine();
            ImGui::TextColored(ParameterColor, ParameterString.c_str());
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
    std::vector<std::string>                                             ParameterStrings;
    ParameterStrings.reserve(ParameterNodes.size());
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        const HTNAtom     Parameter       = GetNodeValue(*ParameterNode);
        const std::string ParameterString = Parameter.ToString(ShouldDoubleQuoteString);
        ParameterStrings.emplace_back(ParameterString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ParameterString : ParameterStrings)
        {
            constexpr ImVec4 ParameterColor = HTNImGuiHelpers::kParameterColor;
            ImGui::SameLine();
            ImGui::TextColored(ParameterColor, ParameterString.c_str());
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

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
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
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            const ImVec4 ArgumentColor = HTNImGuiHelpers::GetArgumentColor(ArgumentString);
            ImGui::SameLine();
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags   TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inConditionNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(IDColor, std::format("#{}", IDString).c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            const ImVec4 ArgumentColor = HTNImGuiHelpers::GetArgumentColor(ArgumentString);
            ImGui::SameLine();
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
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

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
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

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
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

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
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

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        const bool   Result  = inNodeSnapshot.GetResult();
        const ImVec4 IDColor = HTNImGuiHelpers::GetResultColor(Result);
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
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            const ImVec4 ArgumentColor = HTNImGuiHelpers::GetArgumentColor(ArgumentString);
            ImGui::SameLine();
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
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
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            const ImVec4 ArgumentColor = HTNImGuiHelpers::GetArgumentColor(ArgumentString);
            ImGui::SameLine();
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeFunction NodeFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) -> HTNDecompositionNode {
        return HTNDecompositionNode(inNodeSnapshot, ArgumentNodes, mCurrentVariableScopeNodePath);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    constexpr ImGuiTreeNodeFlags   TreeNodeFlags        = ImGuiTreeNodeFlags_Leaf;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeTitleFunction, NodeBehaviorFunction, NodeFunction, TreeNodeFlags);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode)
{
    const HTNAtom&     Value                   = inIdentifierExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = false;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    const HTNAtom&     Value                   = inLiteralExpressionNode.GetValue();
    constexpr bool     ShouldDoubleQuoteString = true;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    const std::string VariableValueString = inVariableExpressionNode.ToString();
    return std::format("?{}", VariableValueString);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    const std::string ConstantValueString = inConstantExpressionNode.ToString();
    return std::format("@{}", ConstantValueString);
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

    const HTNNodeSnapshotCollectionDebug& NodeSnapshotCollection = NodeSnapshotHistory->GetNodeSnapshotCollection();

    ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | inTreeNodeFlags;

    const bool IsChoicePoint = NodeSnapshotHistory->IsChoicePoint();
    if (IsChoicePoint)
    {
        TreeNodeFlags &= ~ImGuiTreeNodeFlags_Leaf;

        int CurrentDecompositionStep = -1;
        for (auto It = NodeSnapshotCollection.begin(); It != NodeSnapshotCollection.end(); ++It)
        {
            const std::size_t           DecompositionStep = It->first;
            const HTNNodeSnapshotDebug& NodeSnapshot      = It->second;

            const std::string Label = std::format("##{}{}", CurrentNodePath, DecompositionStep);

            ImGuiTreeNodeFlags CurrentTreeNodeFlags = TreeNodeFlags;
            if (mCurrentDecompositionStep == DecompositionStep)
            {
                CurrentTreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
            }

            if (IsNodeSelected(NodeSnapshot))
            {
                HTNImGuiHelpers::SelectTreeNode(CurrentTreeNodeFlags);
            }

            if (HTNImGuiHelpers::IsTreeNodeOpen(Label.c_str(), CurrentTreeNodeFlags))
            {
                CurrentDecompositionStep = static_cast<int>(DecompositionStep);
                break;
            }
        }

        SetCurrentDecompositionStep(CurrentDecompositionStep);
    }

    const int         PreviousMinDecompositionStep = mMinDecompositionStep;
    const std::size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    int PreviousDecompositionStep = -1;
    for (auto It = NodeSnapshotCollection.begin(); It != NodeSnapshotCollection.end(); ++It)
    {
        const std::size_t           DecompositionStep = It->first;
        const HTNNodeSnapshotDebug& NodeSnapshot      = It->second;

        if (!IsValidDecompositionStep(static_cast<int>(DecompositionStep), IsChoicePoint))
        {
            PreviousDecompositionStep = static_cast<int>(DecompositionStep);
            continue;
        }

        const std::string Label = std::format("##{}{}", CurrentNodePath, DecompositionStep);

        ImGuiTreeNodeFlags CurrentTreeNodeFlags = TreeNodeFlags;
        if (mCurrentDecompositionStep == DecompositionStep)
        {
            CurrentTreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(CurrentTreeNodeFlags);
        }

        const bool IsOpen = ImGui::TreeNodeEx(Label.c_str(), CurrentTreeNodeFlags);

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

        // TODO salvarez Draw background for invalid decomposition
        // TODO salvarez Fix axioms -> Record video

        // TODO salvarez
        // if (NodeSnapshotCollectionSize > 1)
        //{
        // if (-1 == mCurrentDecompositionStep)
        //{
        ImGui::SameLine();
        ImGui::TextDisabled("%i", DecompositionStep);
        //}
        //}

        inNodeTitleFunction(NodeSnapshot);

        if (!IsOpen)
        {
            PreviousDecompositionStep = static_cast<int>(DecompositionStep);
            continue;
        }

        const int         CurrentMinDecompositionStep = PreviousDecompositionStep;
        const std::size_t CurrentMaxDecompositionStep = DecompositionStep;
        SetMinDecompositionStep(CurrentMinDecompositionStep);
        SetMaxDecompositionStep(CurrentMaxDecompositionStep);

        if (inNodeBehaviorFunction)
        {
            (*inNodeBehaviorFunction)();
        }

        SetMinDecompositionStep(PreviousMinDecompositionStep);
        SetMaxDecompositionStep(PreviousMaxDecompositionStep);

        if (TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen)
        {
            PreviousDecompositionStep = static_cast<int>(DecompositionStep);
            continue;
        }

        ImGui::TreePop();
    }

    return true;
}
#endif
