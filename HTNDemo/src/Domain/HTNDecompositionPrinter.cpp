#include "Domain/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG
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

namespace
{
void PrintKeyword(const std::string& inKeyword)
{
    ImGui::TextDisabled(inKeyword.c_str());
}

std::string MakeDecompositionStepLabel(const std::string& inLabel, const size_t inDecompositionStep)
{
    return std::format("{}{}", inDecompositionStep, inLabel);
}
} // namespace

bool HTNDecompositionPrinter::Print(HTNDecompositionNode& ioSelectedNode)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    mSelectedNode = ioSelectedNode;

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

    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();
    std::vector<std::string>                                             ParameterStrings;
    ParameterStrings.reserve(ParameterNodes.size());
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        const HTNAtom     Parameter       = GetNodeValue(*ParameterNode);
        const std::string ParameterString = Parameter.ToString(ShouldDoubleQuoteString);
        ParameterStrings.emplace_back(ParameterString);
        NodeDescription.append(std::format(" {}", ParameterString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 IDColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::string& ParameterString : ParameterStrings)
        {
            ImGui::SameLine();
            ImGui::TextColored(HTNImGuiHelpers::kParametersColor, ParameterString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ParameterNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
        {
            GetNodeValue(*ConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAxiomNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodVariableScopeNodeScope = HTNNodeScope(mCurrentVariableScopeNodePath, inMethodNode.GetID());

    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inMethodNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();
    std::vector<std::string>                                             ParameterStrings;
    ParameterStrings.reserve(ParameterNodes.size());
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        const HTNAtom     Parameter       = GetNodeValue(*ParameterNode);
        const std::string ParameterString = Parameter.ToString(ShouldDoubleQuoteString);
        ParameterStrings.emplace_back(ParameterString);
        NodeDescription.append(std::format(" {}", ParameterString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ParameterString : ParameterStrings)
        {
            ImGui::SameLine();
            ImGui::TextColored(HTNImGuiHelpers::kParametersColor, ParameterString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ParameterNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            GetNodeValue(*BranchNode);
        }
    };

    return PrintNodeSnapshotHistory(inMethodNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inBranchNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

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

    return PrintNodeSnapshotHistory(inBranchNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode)
{
    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 IDColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(IDColor, IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            ImGui::SameLine();
            const ImVec4 ArgumentColor =
                HTNDecompositionHelpers::IsParameter(ArgumentString) ? HTNImGuiHelpers::kParametersColor : HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    return PrintNodeSnapshotHistory(inConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    std::string                                               NodeDescription         = "#";
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription.append(IDString);

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 IDColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(IDColor, std::format("#{}", IDString).c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            ImGui::SameLine();
            const ImVec4 ArgumentColor =
                HTNDecompositionHelpers::IsParameter(ArgumentString) ? HTNImGuiHelpers::kParametersColor : HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomConditionNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode);

        const std::string                         AxiomNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode> AxiomNode   = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode);
    };

    return PrintNodeSnapshotHistory(inAxiomConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const std::string NodeDescription = "and";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 NodeDescriptionColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(NodeDescriptionColor, NodeDescription.c_str());
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAndConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const std::string NodeDescription = "or";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 NodeDescriptionColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(NodeDescriptionColor, NodeDescription.c_str());
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inOrConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const std::string NodeDescription = "alt";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 NodeDescriptionColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(NodeDescriptionColor, NodeDescription.c_str());
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAltConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const std::string NodeDescription = "not";

    const HTNNodeTitleFunction NodeTitleFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        const ImVec4 NodeDescriptionColor = inNodeSnapshot.GetResult() ? HTNImGuiHelpers::kSuccessColor : HTNImGuiHelpers::kFailColor;
        ImGui::TextColored(NodeDescriptionColor, NodeDescription.c_str());
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode);
    };

    return PrintNodeSnapshotHistory(inNotConditionNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inCompoundTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            ImGui::SameLine();
            const ImVec4 ArgumentColor =
                HTNDecompositionHelpers::IsParameter(ArgumentString) ? HTNImGuiHelpers::kParametersColor : HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
        const HTNAtom                                             ID     = GetNodeValue(*IDNode);

        const std::string                          MethodNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode> MethodNode   = mDomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode);
    };

    return PrintNodeSnapshotHistory(inCompoundTaskNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    std::string                                               NodeDescription;
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode                  = inPrimitiveTaskNode.GetIDNode();
    const HTNAtom                                             ID                      = GetNodeValue(*IDNode);
    constexpr bool                                            ShouldDoubleQuoteString = false;
    const std::string                                         IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                                   = IDString;

    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    std::vector<std::string>                                              ArgumentStrings;
    ArgumentStrings.reserve(ArgumentNodes.size());
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        ArgumentStrings.emplace_back(ArgumentString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeTitleFunction NodeTitleFunction = [&](MAYBE_UNUSED const HTNNodeSnapshotDebug& inNodeSnapshot) {
        ImGui::SameLine();
        ImGui::Text(IDString.c_str());

        for (const std::string& ArgumentString : ArgumentStrings)
        {
            ImGui::SameLine();
            const ImVec4 ArgumentColor =
                HTNDecompositionHelpers::IsParameter(ArgumentString) ? HTNImGuiHelpers::kParametersColor : HTNImGuiHelpers::kArgumentsColor;
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
        }
    };

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeDescription, NodeTitleFunction, NodeSelectionFunction, NodeBehaviorFunction);
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

bool HTNDecompositionPrinter::PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const std::string& inNodeDescription,
                                                       const HTNNodeTitleFunction&     inNodeTitleFunction,
                                                       const HTNNodeSelectionFunction& inNodeSelectionFunction,
                                                       const HTNNodeBehaviorFunction*  inNodeBehaviorFunction)
{
    const HTNNodeScope NodeScope = HTNNodeScope(mCurrentNodePath, inNode.GetID());

    const std::string&                 CurrentNodePath     = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* NodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!NodeSnapshotHistory)
    {
        return false;
    }

    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = NodeSnapshotHistory->begin(); It != NodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (!inNodeBehaviorFunction)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (It == --NodeSnapshotHistory->end())
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string Label                  = HTNImGuiHelpers::MakeLabel(inNodeDescription, CurrentNodePath);
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(Label, DecompositionStep);
        const bool        IsOpen                 = ImGui::TreeNodeEx(DecompositionStepLabel.c_str(), TreeNodeFlags);

        if (inNodeTitleFunction)
        {
            inNodeTitleFunction(NodeSnapshot);
        }

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            if (inNodeSelectionFunction)
            {
                inNodeSelectionFunction(NodeSnapshot);
            }
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep = (NextIt != NodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        if (inNodeBehaviorFunction)
        {
            (*inNodeBehaviorFunction)();
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}
#endif
