#include "Domain/HTNDecompositionPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNNodeScope.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueNode.h"
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
    return std::format("{} {}", inDecompositionStep, inLabel);
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

    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inAxiomNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
        {
            GetNodeValue(*ConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAxiomNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodVariableScopeNodeScope = HTNNodeScope(mCurrentVariableScopeNodePath, inMethodNode.GetID());

    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inMethodNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inMethodNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            GetNodeValue(*BranchNode);
        }
    };

    return PrintNodeSnapshotHistory(inMethodNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inBranchNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

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

    return PrintNodeSnapshotHistory(inBranchNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode)
{
    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inConditionNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    return PrintNodeSnapshotHistory(inConditionNode, NodeDescription, NodeSelectionFunction, NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inAxiomConditionNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomConditionNode.GetIDNode();
        const HTNAtom                              ID     = GetNodeValue(*IDNode);

        const std::string                         AxiomNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNAxiomNode> AxiomNode   = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode);
    };

    return PrintNodeSnapshotHistory(inAxiomConditionNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const std::string NodeDescription = "and";

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAndConditionNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const std::string NodeDescription = "or";

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inOrConditionNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const std::string NodeDescription = "alt";

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }
    };

    return PrintNodeSnapshotHistory(inAltConditionNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const std::string NodeDescription = "not";

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) { SelectNode(inNodeSnapshot); };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode);
    };

    return PrintNodeSnapshotHistory(inNotConditionNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inCompoundTaskNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction NodeBehaviorFunction = [&]() {
        const std::shared_ptr<const HTNValueNode>& IDNode = inCompoundTaskNode.GetIDNode();
        const HTNAtom                              ID     = GetNodeValue(*IDNode);

        const std::string                          MethodNodeID = ID.GetValue<std::string>();
        const std::shared_ptr<const HTNMethodNode> MethodNode   = mDomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode);
    };

    return PrintNodeSnapshotHistory(inCompoundTaskNode, NodeDescription, NodeSelectionFunction, &NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    std::string                                NodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode                  = inPrimitiveTaskNode.GetIDNode();
    const HTNAtom                              ID                      = GetNodeValue(*IDNode);
    constexpr bool                             ShouldDoubleQuoteString = false;
    const std::string                          IDString                = ID.ToString(ShouldDoubleQuoteString);
    NodeDescription                                                    = IDString;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        const HTNAtom     Argument       = GetNodeValue(*ArgumentNode);
        const std::string ArgumentString = Argument.ToString(ShouldDoubleQuoteString);
        NodeDescription.append(std::format(" {}", ArgumentString));
    }

    const HTNNodeSelectionFunction NodeSelectionFunction = [&](const HTNNodeSnapshotDebug& inNodeSnapshot) {
        SelectNode(inNodeSnapshot, ArgumentNodes);
    };

    const HTNNodeBehaviorFunction* NodeBehaviorFunction = nullptr;
    return PrintNodeSnapshotHistory(inPrimitiveTaskNode, NodeDescription, NodeSelectionFunction, NodeBehaviorFunction);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNValueNode& inValueNode)
{
    const HTNAtom&     Value                   = inValueNode.GetValue();
    const bool         ShouldDoubleQuoteString = !inValueNode.IsIdentifier();
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const std::string VariableValueString = inVariableValueNode.ToString();
    return std::format("?{}", VariableValueString);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string ConstantValueString = inConstantValueNode.ToString();
    return std::format("@{}", ConstantValueString);
}

bool HTNDecompositionPrinter::PrintNodeSnapshotHistory(const HTNNodeBase& inNode, const std::string& inNodeDescription,
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

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(inNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

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
