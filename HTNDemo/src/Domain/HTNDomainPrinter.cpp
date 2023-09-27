#include "Domain/HTNDomainPrinter.h"

#include "HTNLog.h"
#include "Domain/AST/HTNAxiomNode.h"
#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNConstantNode.h"
#include "Domain/AST/HTNConstantsNode.h"
#include "Domain/AST/HTNDomainNode.h"
#include "Domain/AST/HTNMethodNode.h"
#include "Domain/AST/HTNTaskNode.h"
#include "Domain/AST/HTNValueNode.h"

#include "imgui.h"

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

HTNDomainPrinter::HTNDomainPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode) : mDomainNode(inDomainNode)
{
}

HTNDomainPrinter::~HTNDomainPrinter() = default;

bool HTNDomainPrinter::Print()
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode).GetValue<bool>();
}

HTNAtom HTNDomainPrinter::Visit(const HTNDomainNode& inDomainNode)
{
    PrintKeyword("domain");

    const std::shared_ptr<const HTNValueNode>& IDNode = inDomainNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    if (inDomainNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& ConstantsNodes = inDomainNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantsNode : ConstantsNodes)
    {
        GetNodeValue(*ConstantsNode);
    }

    const std::vector<std::shared_ptr<const HTNAxiomNode>>& AxiomNodes = inDomainNode.GetAxiomNodes();
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : AxiomNodes)
    {
        GetNodeValue(*AxiomNode);
    }

    const std::vector<std::shared_ptr<const HTNMethodNode>>& MethodNodes = inDomainNode.GetMethodNodes();
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : MethodNodes)
    {
        GetNodeValue(*MethodNode);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantsNode& inConstantsNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    const std::shared_ptr<const HTNValueNode>& IDNode = inConstantNode.GetIDNode();
    GetNodeValue(*IDNode);

    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    ImGui::SameLine();
    GetNodeValue(*ArgumentNode);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNMethodNode& inMethodNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNBranchNode& inBranchNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNConditionNode& inConditionNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    PrintKeyword("axiom");

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomConditionNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
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

HTNAtom HTNDomainPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    PrintKeyword("not");

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    ImGui::SameLine();
    GetNodeValue(*SubConditionNode);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
{
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

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    PrintKeyword("primitive");

    const std::shared_ptr<const HTNValueNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    ImGui::SameLine();
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

HTNAtom HTNDomainPrinter::Visit(const HTNValueNode& inValueNode)
{
    ImGui::Text(inValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    PrintKeyword("variable");

    ImGui::SameLine();
    ImGui::Text(inVariableValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    PrintKeyword("constant");

    ImGui::SameLine();
    ImGui::Text(inConstantValueNode.ToString().c_str());

    return true;
}
