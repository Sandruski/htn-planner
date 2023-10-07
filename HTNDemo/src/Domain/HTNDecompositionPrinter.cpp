#include "Domain/HTNDecompositionPrinter.h"

#include "Domain/HTNDomainHelpers.h"
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
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNLog.h"

#include "imgui.h"

#include <cassert>
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

std::string MakeLabel(const std::string& inLabel, const std::string& inID)
{
    return std::format("{}##{}", inLabel, inID);
}

const ImGuiTreeNodeFlags DefaultTreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
} // namespace

HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                 const HTNDecompositionSnapshotDebug& inDecompositionSnapshot)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot)
{
}

bool HTNDecompositionPrinter::Print(const HTNNodeSnapshotDebug*& ioSelectedNodeSnapshot)
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    mSelectedNodeSnapshot = ioSelectedNodeSnapshot;

    GetNodeValue(*DomainNode).GetValue<bool>();

    ioSelectedNodeSnapshot = mSelectedNodeSnapshot;

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNDomainNode& inDomainNode)
{
    const HTNNodeScope DomainNodeScope = HTNNodeScope(mDecompositionContext, inDomainNode.GetID());

    const std::string&                 DomainNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* DomainNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(DomainNodePath);
    if (!DomainNodeSnapshotHistory)
    {
        return false;
    }

    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDomainHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    GetNodeValue(*TopLevelCompoundTaskNode);

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantsNode& inConstantsNode)
{
    const HTNNodeScope ConstantsNodeScope = HTNNodeScope(mDecompositionContext, inConstantsNode.GetID());

    const std::string&                 ConstantsNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ConstantsNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ConstantsNodePath);
    if (!ConstantsNodeSnapshotHistory)
    {
        return false;
    }

    // TODO salvarez
    /*
    PrintKeyword("constants");

    if (const std::shared_ptr<const HTNValueNode>& IDNode = inConstantsNode.GetIDNode())
    {
        ImGui::SameLine();
        GetNodeValue(*IDNode);
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodes = inConstantsNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodes)
    {
        GetNodeValue(*ConstantNode);
    }
    ImGui::Unindent();
    */

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    const HTNNodeScope ConstantNodeScope = HTNNodeScope(mDecompositionContext, inConstantNode.GetID());

    const std::string&                 ConstantNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ConstantNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ConstantNodePath);
    if (!ConstantNodeSnapshotHistory)
    {
        return false;
    }

    // TODO salvarez
    /*
    const std::shared_ptr<const HTNValueNode>& IDNode = inConstantNode.GetIDNode();
    GetNodeValue(*IDNode);

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    ImGui::SameLine();
    GetNodeValue(*ArgumentNode);
    */

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
    const HTNNodeScope AxiomNodeScope = HTNNodeScope(mDecompositionContext, inAxiomNode.GetID());

    const std::string&                 AxiomNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* AxiomNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(AxiomNodePath);
    assert(AxiomNodeSnapshotHistory);

    if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
    {
        GetNodeValue(*ConditionNode);
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    const HTNNodeScope MethodNodeScope = HTNNodeScope(mDecompositionContext, inMethodNode.GetID());

    const std::string&                 MethodNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* MethodNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(MethodNodePath);
    assert(MethodNodeSnapshotHistory);

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
    {
        GetNodeValue(*BranchNode);
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    const HTNNodeScope BranchNodeScope = HTNNodeScope(mDecompositionContext, inBranchNode.GetID());

    const std::string&                 BranchNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* BranchNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(BranchNodePath);
    if (!BranchNodeSnapshotHistory)
    {
        return false;
    }

    // Make branch node description
    const std::shared_ptr<const HTNValueNode>& BranchNodeIDNode      = inBranchNode.GetIDNode();
    const std::string                          BranchNodeID          = GetNodeValue(*BranchNodeIDNode).GetValue<std::string>();
    const std::string                          BranchNodeDescription = BranchNodeID;

    // Print branch node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& BranchNodeSnapshotPair : *BranchNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = BranchNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(BranchNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, BranchNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
            {
                GetNodeValue(*PreConditionNode);
            }

            const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
            for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
            {
                GetNodeValue(*TaskNode);
            }

            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConditionNode& inConditionNode)
{
    const HTNNodeScope ConditionNodeScope = HTNNodeScope(mDecompositionContext, inConditionNode.GetID());

    const std::string&                 ConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ConditionNodePath);
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
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& ConditionNodeSnapshotPair : *ConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = ConditionNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(ConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, ConditionNodePath);
        if (ImGui::TreeNodeEx(Label.c_str(), ImGuiTreeNodeFlags_Leaf))
        {
            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    const HTNNodeScope AxiomConditionNodeScope = HTNNodeScope(mDecompositionContext, inAxiomConditionNode.GetID());

    const std::string&                 AxiomConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* AxiomConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(AxiomConditionNodePath);
    if (!AxiomConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make axiom condition node description
    const std::shared_ptr<const HTNValueNode>& AxiomConditionNodeIDNode      = inAxiomConditionNode.GetIDNode();
    const std::string                          AxiomNodeID                   = GetNodeValue(*AxiomConditionNodeIDNode).GetValue<std::string>();
    std::string                                AxiomConditionNodeDescription = std::format("#{}", AxiomNodeID);

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& AxiomConditionNodeArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& AxiomConditionNodeArgumentNode : AxiomConditionNodeArgumentNodes)
    {
        AxiomConditionNodeDescription.append(std::format(" {}", GetNodeValue(*AxiomConditionNodeArgumentNode).GetValue<std::string>()));
    }

    // Print axiom condition node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& AxiomConditionNodeSnapshotPair : *AxiomConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = AxiomConditionNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AxiomConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, AxiomConditionNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            // Call axiom node
            const std::shared_ptr<const HTNAxiomNode> AxiomNode = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
            GetNodeValue(*AxiomNode);
            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    const HTNNodeScope AndConditionNodeScope = HTNNodeScope(mDecompositionContext, inAndConditionNode.GetID());

    const std::string&                 AndConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* AndConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(AndConditionNodePath);
    if (!AndConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make and condition node description
    const std::string AndConditionNodeID          = "and";
    const std::string AndConditionNodeDescription = AndConditionNodeID;

    // Print and condition node snapshot history
    for (auto It = AndConditionNodeSnapshotHistory->begin(); It != AndConditionNodeSnapshotHistory->end(); ++It)
    {
        ImGuiTreeNodeFlags          TreeNodeFlags = DefaultTreeNodeFlags;
        const HTNNodeSnapshotDebug& NodeSnapshot  = It->second;
        const bool                  IsSelected    = mSelectedNodeSnapshot == &NodeSnapshot;
        if (IsSelected)
        {
            TreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        const std::size_t DecompositionStep      = It->first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AndConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, AndConditionNodePath);
        const bool        IsOpen                 = ImGui::TreeNodeEx(Label.c_str(), TreeNodeFlags);

        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && !ImGui::IsItemToggledOpen())
        {
            mSelectedNodeSnapshot = &NodeSnapshot;
        }

        if (IsOpen)
        {
            const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
            for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
            {
                GetNodeValue(*SubConditionNode);
            }

            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    const HTNNodeScope OrConditionNodeScope = HTNNodeScope(mDecompositionContext, inOrConditionNode.GetID());

    const std::string&                 OrConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* OrConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(OrConditionNodePath);
    if (!OrConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make or condition node description
    const std::string OrConditionNodeID          = "or";
    const std::string OrConditionNodeDescription = OrConditionNodeID;

    // Print or condition node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& OrConditionNodeSnapshotPair : *OrConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = OrConditionNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(OrConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, OrConditionNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
            for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
            {
                GetNodeValue(*SubConditionNode);
            }

            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    const HTNNodeScope AltConditionNodeScope = HTNNodeScope(mDecompositionContext, inAltConditionNode.GetID());

    const std::string&                 AltConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* AltConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(AltConditionNodePath);
    if (!AltConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make alt condition node description
    const std::string AltConditionNodeID          = "alt";
    const std::string AltConditionNodeDescription = AltConditionNodeID;

    // Print alt condition node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& AltConditionNodeSnapshotPair : *AltConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = AltConditionNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(AltConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, AltConditionNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
            for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
            {
                GetNodeValue(*SubConditionNode);
            }

            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    const HTNNodeScope NotConditionNodeScope = HTNNodeScope(mDecompositionContext, inNotConditionNode.GetID());

    const std::string&                 NotConditionNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* NotConditionNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(NotConditionNodePath);
    if (!NotConditionNodeSnapshotHistory)
    {
        return false;
    }

    // Make not condition node description
    const std::string NotConditionNodeID          = "not";
    const std::string NotConditionNodeDescription = NotConditionNodeID;

    // Print not condition node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& NotConditionNodeSnapshotPair : *NotConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = NotConditionNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(NotConditionNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, NotConditionNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
            GetNodeValue(*SubConditionNode);
            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
    const HTNNodeScope CompoundTaskNodeScope = HTNNodeScope(mDecompositionContext, inCompoundTaskNode.GetID());

    const std::string&                 CompoundTaskNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* CompoundTaskNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(CompoundTaskNodePath);
    if (!CompoundTaskNodeSnapshotHistory)
    {
        return false;
    }

    // Make compound task node description
    const std::shared_ptr<const HTNValueNode>& CompoundTaskNodeIDNode      = inCompoundTaskNode.GetIDNode();
    const std::string                          MethodNodeID                = GetNodeValue(*CompoundTaskNodeIDNode).GetValue<std::string>();
    std::string                                CompoundTaskNodeDescription = MethodNodeID;

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& CompoundTaskNodeArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& CompoundTaskNodeArgumentNode : CompoundTaskNodeArgumentNodes)
    {
        CompoundTaskNodeDescription.append(std::format(" {}", GetNodeValue(*CompoundTaskNodeArgumentNode).GetValue<std::string>()));
    }

    // Print compound task node snapshot history
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& CompoundTaskNodeSnapshotPair : *CompoundTaskNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = CompoundTaskNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(CompoundTaskNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, CompoundTaskNodePath);
        if (ImGui::TreeNode(Label.c_str()))
        {
            // Call method node
            const std::shared_ptr<const HTNMethodNode> MethodNode = mDomainNode->FindMethodNodeByID(MethodNodeID);
            GetNodeValue(*MethodNode);
            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    const HTNNodeScope PrimitiveTaskNodeScope = HTNNodeScope(mDecompositionContext, inPrimitiveTaskNode.GetID());

    const std::string&                 PrimitiveTaskNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* PrimitiveTaskNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(PrimitiveTaskNodePath);
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
    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& PrimitiveTaskNodeSnapshotPair : *PrimitiveTaskNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep      = PrimitiveTaskNodeSnapshotPair.first;
        const std::string DecompositionStepLabel = MakeDecompositionStepLabel(PrimitiveTaskNodeDescription, DecompositionStep);
        const std::string Label                  = MakeLabel(DecompositionStepLabel, PrimitiveTaskNodePath);
        if (ImGui::TreeNodeEx(Label.c_str(), ImGuiTreeNodeFlags_Leaf))
        {
            ImGui::TreePop();
        }
    }

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNValueNode& inValueNode)
{
    const HTNNodeScope ValueNodeScope = HTNNodeScope(mDecompositionContext, inValueNode.GetID());

    const std::string&                 ValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ValueNodePath);
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
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    const std::string&                 VariableValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* VariableValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(VariableValueNodePath);
    if (!VariableValueNodeSnapshotHistory)
    {
        return "";
    }

    return std::format("?{}", inVariableValueNode.ToString());
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNNodeScope ConstantValueNodeScope = HTNNodeScope(mDecompositionContext, inConstantValueNode.GetID());

    const std::string&                 ConstantValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ConstantValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ConstantValueNodePath);
    if (!ConstantValueNodeSnapshotHistory)
    {
        return "";
    }

    return std::format("@{}", inConstantValueNode.ToString());

    // TODO salvarez
    // Call constant node
    /*
    const std::string& ConstantNodeID = inConstantValueNode.GetConstantNodeID();
    const std::string  TreeNodeID     = std::format("{}##{}", ConstantNodeID, ConstantValueNodePath);
    if (ImGui::TreeNode(TreeNodeID.c_str()))
    {
        std::shared_ptr<const HTNConstantNode> ConstantNode = mDomainNode->FindConstantNodeByID(ConstantNodeID);
        GetNodeValue(*ConstantNode);
        ImGui::TreePop();
    }
    */
}
