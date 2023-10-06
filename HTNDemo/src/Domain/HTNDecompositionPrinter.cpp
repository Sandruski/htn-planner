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

void PrintTopLevelKeyword()
{
    ImGui::SameLine();
    PrintKeyword("top_level");
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
    PrintKeyword("domain");

    const std::shared_ptr<const HTNValueNode>& IDNode = inDomainNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    if (inDomainNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

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

    const std::shared_ptr<const HTNValueNode>& IDNode = inConstantNode.GetIDNode();
    GetNodeValue(*IDNode);

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    ImGui::SameLine();
    GetNodeValue(*ArgumentNode);

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

    PrintKeyword("axiom");

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }

    if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
    {
        GetNodeValue(*ConditionNode);
    }
    ImGui::Unindent();

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

    PrintKeyword("method");

    const std::shared_ptr<const HTNValueNode>& IDNode = inMethodNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    if (inMethodNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inMethodNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
    {
        GetNodeValue(*BranchNode);
    }
    ImGui::Unindent();

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

    const std::shared_ptr<const HTNValueNode>& IDNode = inBranchNode.GetIDNode();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
    {
        GetNodeValue(*PreConditionNode);
    }

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
    {
        GetNodeValue(*TaskNode);
    }
    ImGui::Unindent();

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

    const std::shared_ptr<const HTNValueNode>& IDNode = inConditionNode.GetIDNode();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }
    ImGui::Unindent();

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
    PrintKeyword("#");

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomConditionNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }
    ImGui::Unindent();

    // Call axiom node
    const std::string& AxiomNodeID = inAxiomConditionNode.GetAxiomNodeID();
    const std::string  TreeNodeID  = std::format("{}##{}", AxiomNodeID, AxiomConditionNodePath);
    if (ImGui::TreeNode(TreeNodeID.c_str()))
    {
        const std::shared_ptr<const HTNAxiomNode> AxiomNode = mDomainNode->FindAxiomNodeByID(AxiomNodeID);
        GetNodeValue(*AxiomNode);
        ImGui::TreePop();
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

    PrintKeyword("and");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
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

    PrintKeyword("or");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
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

    PrintKeyword("alt");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
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

    PrintKeyword("not");

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    ImGui::SameLine();
    GetNodeValue(*SubConditionNode);

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

    // Print compound task node
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

    // Call method node
    const std::string& MethodNodeID = inCompoundTaskNode.GetMethodNodeID();
    const std::string  TreeNodeID   = std::format("{}##{}", MethodNodeID, CompoundTaskNodePath);
    if (ImGui::TreeNode(TreeNodeID.c_str()))
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

    PrintKeyword("!");

    const std::shared_ptr<const HTNValueNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNValueNode& inValueNode)
{
    const HTNNodeScope ValueNodeScope = HTNNodeScope(mDecompositionContext, inValueNode.GetID());

    const std::string&                 ValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ValueNodePath);
    if (!ValueNodeSnapshotHistory)
    {
        return false;
    }

    const HTNAtom&    Value                   = inValueNode.GetValue();
    const bool        ShouldDoubleQuoteString = !inValueNode.IsIdentifier();
    const std::string ValueString             = Value.ToString(ShouldDoubleQuoteString);
    ImGui::Text(ValueString.c_str());

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const HTNNodeScope VariableValueNodeScope = HTNNodeScope(mDecompositionContext, inVariableValueNode.GetID());

    const std::string&                 VariableValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* VariableValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(VariableValueNodePath);
    if (!VariableValueNodeSnapshotHistory)
    {
        return false;
    }

    PrintKeyword("?");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inVariableValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNDecompositionPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const HTNNodeScope ConstantValueNodeScope = HTNNodeScope(mDecompositionContext, inConstantValueNode.GetID());

    const std::string&                 ConstantValueNodePath            = mDecompositionContext.GetCurrentNodePath();
    const HTNNodeSnapshotHistoryDebug* ConstantValueNodeSnapshotHistory = mDecompositionSnapshot.FindNodeSnapshotHistory(ConstantValueNodePath);
    if (!ConstantValueNodeSnapshotHistory)
    {
        return false;
    }

    // Print constant value node
    PrintKeyword("@");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inConstantValueNode.ToString().c_str());

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

    return true;
}
