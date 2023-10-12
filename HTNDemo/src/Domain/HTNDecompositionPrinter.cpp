#include "Domain/HTNDecompositionPrinter.h"

#include "Domain/HTNDomainHelpers.h"
#include "Domain/HTNNodeScope.h"
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
#include "HTNLog.h"

#include "imgui.h"

#include <format>

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

HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                 const HTNDecompositionSnapshotDebug& inDecompositionSnapshot)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot)
{
}

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
    const HTNNodeScope DomainNodeScope = HTNNodeScope(mCurrentNodePath, inDomainNode.GetID());

    const std::string&                 CurrentNodePath           = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* DomainNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!DomainNodeSnapshotHistory)
    {
        return false;
    }

    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDomainHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    GetNodeValue(*TopLevelCompoundTaskNode);

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNNodeScope AxiomNodeScope = HTNNodeScope(mCurrentNodePath, inAxiomNode.GetID());

    const std::string&                 CurrentNodePath          = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* AxiomNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    assert(AxiomNodeSnapshotHistory);

    // Make axiom node description
    const std::shared_ptr<const HTNValueNode>& AxiomNodeIDNode      = inAxiomNode.GetIDNode();
    const std::string                          AxiomNodeID          = GetNodeValue(*AxiomNodeIDNode).GetValue<std::string>();
    std::string                                AxiomNodeDescription = AxiomNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomNodeArgumentNodes = inAxiomNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& AxiomNodeArgumentNode : AxiomNodeArgumentNodes)
    {
        AxiomNodeDescription.append(std::format(" {}", GetNodeValue(*AxiomNodeArgumentNode).GetValue<std::string>()));
    }

    // Print axiom node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = AxiomNodeSnapshotHistory->begin(); It != AxiomNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --AxiomNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AxiomNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, AxiomNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != AxiomNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
        {
            GetNodeValue(*ConditionNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodNodeScope = HTNNodeScope(mCurrentNodePath, inMethodNode.GetID());

    const std::string&                 CurrentNodePath           = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* MethodNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    assert(MethodNodeSnapshotHistory);

    // Make method node description
    const std::shared_ptr<const HTNValueNode>& MethodNodeIDNode      = inMethodNode.GetIDNode();
    const std::string                          MethodNodeID          = GetNodeValue(*MethodNodeIDNode).GetValue<std::string>();
    std::string                                MethodNodeDescription = MethodNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& MethodNodeArgumentNodes = inMethodNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& MethodNodeArgumentNode : MethodNodeArgumentNodes)
    {
        MethodNodeDescription.append(std::format(" {}", GetNodeValue(*MethodNodeArgumentNode).GetValue<std::string>()));
    }

    // Print method node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = MethodNodeSnapshotHistory->begin(); It != MethodNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --MethodNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(MethodNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, MethodNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != MethodNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            GetNodeValue(*BranchNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    const HTNNodeScope BranchNodeScope = HTNNodeScope(mCurrentNodePath, inBranchNode.GetID());

    const std::string&                 CurrentNodePath           = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* BranchNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!BranchNodeSnapshotHistory)
    {
        return false;
    }

    // Make branch node description
    const std::shared_ptr<const HTNValueNode>& BranchNodeIDNode      = inBranchNode.GetIDNode();
    const std::string                          BranchNodeID          = GetNodeValue(*BranchNodeIDNode).GetValue<std::string>();
    const std::string                          BranchNodeDescription = BranchNodeID;

    // Print branch node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = BranchNodeSnapshotHistory->begin(); It != BranchNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --BranchNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(BranchNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != BranchNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
        {
            GetNodeValue(*PreConditionNode);
        }

        const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
        for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
        {
            GetNodeValue(*TaskNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode)
{
    const HTNNodeScope ConditionNodeScope = HTNNodeScope(mCurrentNodePath, inConditionNode.GetID());

    const std::string&                 CurrentNodePath              = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* ConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!ConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make condition node description
    const std::shared_ptr<const HTNValueNode>& ConditionNodeIDNode      = inConditionNode.GetIDNode();
    const std::string                          ConditionNodeID          = GetNodeValue(*ConditionNodeIDNode).GetValue<std::string>();
    std::string                                ConditionNodeDescription = ConditionNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ConditionNodeArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ConditionNodeArgumentNode : ConditionNodeArgumentNodes)
    {
        ConditionNodeDescription.append(std::format(" {}", GetNodeValue(*ConditionNodeArgumentNode).GetValue<std::string>()));
    }

    // Print condition node snapshot history
    for (auto It = ConditionNodeSnapshotHistory->begin(); It != ConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | ImGuiTreeNodeFlags_Leaf;
        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(ConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, ConditionNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const HTNNodeScope AxiomConditionNodeScope = HTNNodeScope(mCurrentNodePath, inAxiomConditionNode.GetID());

    const std::string&                 CurrentNodePath                   = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* AxiomConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!AxiomConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make axiom condition node description
    const std::shared_ptr<const HTNValueNode>& AxiomConditionNodeIDNode      = inAxiomConditionNode.GetIDNode();
    const std::string                          AxiomConditionNodeID          = GetNodeValue(*AxiomConditionNodeIDNode).GetValue<std::string>();
    std::string                                AxiomConditionNodeDescription = std::format("#{}", AxiomConditionNodeID);

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        AxiomConditionNodeDescription.append(std::format(" {}", GetNodeValue(*AxiomConditionNodeArgumentNode).GetValue<std::string>()));
    }

    // Print axiom condition node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = AxiomConditionNodeSnapshotHistory->begin(); It != AxiomConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --AxiomConditionNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AxiomConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, AxiomConditionNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != AxiomConditionNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::shared_ptr<const HTNAxiomNode> AxiomNode = mDomainNode->FindAxiomNodeByID(AxiomConditionNodeID);
        GetNodeValue(*AxiomNode);

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const HTNNodeScope AndConditionNodeScope = HTNNodeScope(mCurrentNodePath, inAndConditionNode.GetID());

    const std::string&                 CurrentNodePath                 = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* AndConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!AndConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make and condition node description
    const std::string AndConditionNodeID          = "and";
    const std::string AndConditionNodeDescription = AndConditionNodeID;

    // Print and condition node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = AndConditionNodeSnapshotHistory->begin(); It != AndConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --AndConditionNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AndConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != AndConditionNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const HTNNodeScope OrConditionNodeScope = HTNNodeScope(mCurrentNodePath, inOrConditionNode.GetID());

    const std::string&                 CurrentNodePath                = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* OrConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!OrConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make or condition node description
    const std::string OrConditionNodeID          = "or";
    const std::string OrConditionNodeDescription = OrConditionNodeID;

    // Print or condition node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = OrConditionNodeSnapshotHistory->begin(); It != OrConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --OrConditionNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(OrConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != OrConditionNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const HTNNodeScope AltConditionNodeScope = HTNNodeScope(mCurrentNodePath, inAltConditionNode.GetID());

    const std::string&                 CurrentNodePath                 = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* AltConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!AltConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make alt condition node description
    const std::string AltConditionNodeID          = "alt";
    const std::string AltConditionNodeDescription = AltConditionNodeID;

    // Print alt condition node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = AltConditionNodeSnapshotHistory->begin(); It != AltConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --AltConditionNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AltConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != AltConditionNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
        }

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const HTNNodeScope NotConditionNodeScope = HTNNodeScope(mCurrentNodePath, inNotConditionNode.GetID());

    const std::string&                 CurrentNodePath                 = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* NotConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!NotConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make not condition node description
    const std::string NotConditionNodeID          = "not";
    const std::string NotConditionNodeDescription = NotConditionNodeID;

    // Print not condition node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = NotConditionNodeSnapshotHistory->begin(); It != NotConditionNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --NotConditionNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(NotConditionNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != NotConditionNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
        GetNodeValue(*SubConditionNode);

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    const HTNNodeScope CompoundTaskNodeScope = HTNNodeScope(mCurrentNodePath, inCompoundTaskNode.GetID());

    const std::string&                 CurrentNodePath                 = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* CompoundTaskNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!CompoundTaskNodeSnapshotHistory)
    {
        return false;
    }

    // Make compound task node description
    const std::shared_ptr<const HTNValueNode>& CompoundTaskNodeIDNode      = inCompoundTaskNode.GetIDNode();
    const std::string                          CompoundTaskNodeID          = GetNodeValue(*CompoundTaskNodeIDNode).GetValue<std::string>();
    std::string                                CompoundTaskNodeDescription = CompoundTaskNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        CompoundTaskNodeDescription.append(std::format(" {}", GetNodeValue(*CompoundTaskNodeArgumentNode).GetValue<std::string>()));
    }

    // Print compound task node snapshot history
    const size_t PreviousMinDecompositionStep = mMinDecompositionStep;
    const size_t PreviousMaxDecompositionStep = mMaxDecompositionStep;

    for (auto It = CompoundTaskNodeSnapshotHistory->begin(); It != CompoundTaskNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags;
        if (It == --CompoundTaskNodeSnapshotHistory->end())
        {
            HTNImGuiHelpers::DefaultOpenTreeNode(TreeNodeFlags);
        }

        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(CompoundTaskNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, CompoundTaskNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        const size_t CurrentMinDecompositionStep = DecompositionStep;
        auto         NextIt                      = It;
        ++NextIt;
        const size_t CurrentMaxDecompositionStep =
            (NextIt != CompoundTaskNodeSnapshotHistory->end()) ? NextIt->first : std::numeric_limits<std::size_t>::max();
        SetDecompositionStepRange(CurrentMinDecompositionStep, CurrentMaxDecompositionStep);

        const std::shared_ptr<const HTNMethodNode> MethodNode = mDomainNode->FindMethodNodeByID(CompoundTaskNodeID);
        GetNodeValue(*MethodNode);

        SetDecompositionStepRange(PreviousMinDecompositionStep, PreviousMaxDecompositionStep);

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    const HTNNodeScope PrimitiveTaskNodeScope = HTNNodeScope(mCurrentNodePath, inPrimitiveTaskNode.GetID());

    const std::string&                 CurrentNodePath                  = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* PrimitiveTaskNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!PrimitiveTaskNodeSnapshotHistory)
    {
        return false;
    }

    // Make primitive task node description
    const std::shared_ptr<const HTNValueNode>& PrimitiveTaskNodeIDNode      = inPrimitiveTaskNode.GetIDNode();
    const std::string                          PrimitiveTaskNodeID          = GetNodeValue(*PrimitiveTaskNodeIDNode).GetValue<std::string>();
    std::string                                PrimitiveTaskNodeDescription = PrimitiveTaskNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& PrimitiveTaskNodeArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& PrimitiveTaskNodeArgumentNode : PrimitiveTaskNodeArgumentNodes)
    {
        PrimitiveTaskNodeDescription.append(std::format(" {}", GetNodeValue(*PrimitiveTaskNodeArgumentNode).GetValue<std::string>()));
    }

    // Print primitive task node snapshot history
    for (auto It = PrimitiveTaskNodeSnapshotHistory->begin(); It != PrimitiveTaskNodeSnapshotHistory->end(); ++It)
    {
        const std::size_t DecompositionStep = It->first;
        if (!IsDecompositionStepBetweenRange(DecompositionStep))
        {
            continue;
        }

        const HTNNodeSnapshotDebug& NodeSnapshot = It->second;

        ImGuiTreeNodeFlags TreeNodeFlags = HTNImGuiHelpers::kDefaultTreeNodeFlags | ImGuiTreeNodeFlags_Leaf;
        if (IsNodeSelected(NodeSnapshot))
        {
            HTNImGuiHelpers::SelectTreeNode(TreeNodeFlags);
        }

        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(PrimitiveTaskNodeDescription, DecompositionStep);
        const std::string Label                  = HTNImGuiHelpers::MakeLabel(DecompositionStepLabel, CurrentNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (HTNImGuiHelpers::IsCurrentItemSelected())
        {
            SelectNode(NodeSnapshot, PrimitiveTaskNodeArgumentNodes);
        }

        if (!IsOpen)
        {
            continue;
        }

        ImGui::TreePop();
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNValueNode& inValueNode)
{
    const HTNNodeScope ValueNodeScope = HTNNodeScope(mCurrentNodePath, inValueNode.GetID());

    const std::string&                 CurrentNodePath          = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* ValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!ValueNodeSnapshotHistory)
    {
        return "";
    }

    const HTNAtom& Value                   = inValueNode.GetValue();
    const bool     ShouldDoubleQuoteString = !inValueNode.IsIdentifier();
    return Value.ToString(ShouldDoubleQuoteString);
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mCurrentNodePath, inVariableValueNode.GetID());

    const std::string&                 CurrentNodePath                  = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* VariableValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!VariableValueNodeSnapshotHistory)
    {
        return "";
    }

    return std::format("?{}", inVariableValueNode.ToString());
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNNodeScope ConstantValueNodeScope = HTNNodeScope(mCurrentNodePath, inConstantValueNode.GetID());

    const std::string&                 CurrentNodePath                  = mCurrentNodePath.GetNodePath();
    const HTNNodeSnapshotHistoryDebug* ConstantValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CurrentNodePath);
    if (!ConstantValueNodeSnapshotHistory)
    {
        return "";
    }

    return std::format("@{}", inConstantValueNode.ToString());
}
