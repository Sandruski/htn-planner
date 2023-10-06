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

#include <format>

namespace
{
void PrintKeyword(const std::string& inKeyword)
{
    ImGui::TextDisabled(inKeyword.c_str());
}

std::string MakeDecompositionStepID(const std::string& inID, const size_t inDecompositionStep)
{
    return std::format("{} {}", inDecompositionStep, inID);
}

std::string MakeLabel(const std::string& inLabel, const std::string& inID)
{
    return std::format("{}##{}", inLabel, inID);
}
} // namespace

HTNDecompositionPrinter::HTNDecompositionPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                 const HTNDecompositionSnapshotDebug& inDecompositionSnapshot)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionSnapshot(inDecompositionSnapshot)
{
}

bool HTNDecompositionPrinter::Print()
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode).GetValue<bool>();
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

    // Print domain node
    const std::shared_ptr<const HTNValueNode>& IDNode = inDomainNode.GetIDNode();
    GetNodeValue(*IDNode);

    // Call top-level compound task node
    ImGui::Indent();
    // Top-level compound task node
    const std::shared_ptr<const HTNCompoundTaskNode> TopLevelCompoundTaskNode = HTNDomainHelpers::MakeTopLevelCompoundTaskNode(mEntryPointID);
    GetNodeValue(*TopLevelCompoundTaskNode);
    ImGui::Unindent();

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
    if (!AxiomNodeSnapshotHistory)
    {
        return false;
    }

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
    if (!MethodNodeSnapshotHistory)
    {
        return false;
    }

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

    const std::string BranchNodeID = inBranchNode.GetID();
    const std::string Label        = MakeLabel(BranchNodeID, BranchNodePath);
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

    std::string                                ConditionNodeDescription;
    const std::shared_ptr<const HTNValueNode>& IDNode = inConditionNode.GetIDNode();
    ConditionNodeDescription                          = GetNodeValue(*IDNode).GetValue<std::string>();

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ConditionNodeDescription.append(std::format(" {}", GetNodeValue(*ArgumentNode).GetValue<std::string>()));
    }

    ImGui::Text(ConditionNodeDescription.c_str());

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

    // Print axiom condition node
    std::string        AxiomConditionNodeDescription = "#";
    const std::string& AxiomNodeID                   = inAxiomConditionNode.GetAxiomNodeID();
    AxiomConditionNodeDescription.append(AxiomNodeID);

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        AxiomConditionNodeDescription.append(std::format(" {}", GetNodeValue(*ArgumentNode).GetValue<std::string>()));
    }

    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& AxiomConditionNodeSnapshotPair : *AxiomConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep = AxiomConditionNodeSnapshotPair.first;
        // const HTNNodeSnapshotDebug& NodeSnapshotDebug = AndConditionNodeSnapshotPair.second;

        const std::string DecompositionStepID = MakeDecompositionStepID(AxiomConditionNodeDescription, DecompositionStep);
        const std::string Label               = MakeLabel(DecompositionStepID, AxiomConditionNodePath);
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

    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& AndConditionNodeSnapshotPair : *AndConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep = AndConditionNodeSnapshotPair.first;
        // const HTNNodeSnapshotDebug& NodeSnapshotDebug = AndConditionNodeSnapshotPair.second;

        const std::string AndConditionNodeID  = "and";
        const std::string DecompositionStepID = MakeDecompositionStepID(AndConditionNodeID, DecompositionStep);
        const std::string Label               = MakeLabel(DecompositionStepID, AndConditionNodePath);
        if (ImGui::TreeNode(Label.c_str()))
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

    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& OrConditionNodeSnapshotPair : *OrConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep = OrConditionNodeSnapshotPair.first;
        // const HTNNodeSnapshotDebug& NodeSnapshotDebug = AndConditionNodeSnapshotPair.second;

        const std::string OrConditionNodeID   = "or";
        const std::string DecompositionStepID = MakeDecompositionStepID(OrConditionNodeID, DecompositionStep);
        const std::string Label               = MakeLabel(DecompositionStepID, OrConditionNodePath);
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

    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& AltConditionNodeSnapshotPair : *AltConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep = AltConditionNodeSnapshotPair.first;
        // const HTNNodeSnapshotDebug& NodeSnapshotDebug = AndConditionNodeSnapshotPair.second;

        const std::string AltConditionNodeID  = "alt";
        const std::string DecompositionStepID = MakeDecompositionStepID(AltConditionNodeID, DecompositionStep);
        const std::string Label               = MakeLabel(DecompositionStepID, AltConditionNodePath);
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

    for (const std::pair<std::size_t, HTNNodeSnapshotDebug>& NotConditionNodeSnapshotPair : *NotConditionNodeSnapshotHistory)
    {
        const std::size_t DecompositionStep = NotConditionNodeSnapshotPair.first;
        // const HTNNodeSnapshotDebug& NodeSnapshotDebug = AndConditionNodeSnapshotPair.second;

        const std::string NotConditionNodeID  = "not";
        const std::string DecompositionStepID = MakeDecompositionStepID(NotConditionNodeID, DecompositionStep);
        const std::string Label               = MakeLabel(DecompositionStepID, NotConditionNodePath);
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

    // TODO salvarez
    // Print compound task node
    /*
    const std::shared_ptr<const HTNValueNode>& IDNode = inCompoundTaskNode.GetIDNode();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }
    ImGui::Unindent();
    */

    // Call method node
    const std::string& MethodNodeID = inCompoundTaskNode.GetMethodNodeID();
    const std::string  Label        = MakeLabel(MethodNodeID, CompoundTaskNodePath);
    if (ImGui::TreeNode(Label.c_str()))
    {
        const std::shared_ptr<const HTNMethodNode> MethodNode = mDomainNode->FindMethodNodeByID(MethodNodeID);
        GetNodeValue(*MethodNode);
        ImGui::TreePop();
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

    std::string                                PrimitiveTaskNodeDescription = "!";
    const std::shared_ptr<const HTNValueNode>& IDNode                       = inPrimitiveTaskNode.GetIDNode();
    PrimitiveTaskNodeDescription.append(GetNodeValue(*IDNode).GetValue<std::string>());

    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        PrimitiveTaskNodeDescription.append(std::format(" {}", GetNodeValue(*ArgumentNode).GetValue<std::string>()));
    }

    ImGui::Text(PrimitiveTaskNodeDescription.c_str());

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
