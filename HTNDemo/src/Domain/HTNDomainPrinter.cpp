#include "Domain/HTNDomainPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueNode.h"

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

bool HTNDomainPrinter::Print(const std::shared_ptr<const HTNDomainNode>& inDomainNode)
{
    const HTNDomainNode* DomainNode = inDomainNode.get();
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

    const std::shared_ptr<const HTNValueNodeBase>& ValueNode = inConstantNode.GetValueNode();
    ImGui::SameLine();
    GetNodeValue(*ValueNode);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
    PrintKeyword("axiom");

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ParameterNodes = inAxiomNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode);
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
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ParameterNodes = inMethodNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNValueNodeBase>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode);
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

HTNAtom HTNDomainPrinter::Visit(const HTNValueNode& inValueNode)
{
    const HTNAtom&    Value                   = inValueNode.GetValue();
    const bool        ShouldDoubleQuoteString = !inValueNode.IsIdentifier();
    const std::string ValueString             = Value.ToString(ShouldDoubleQuoteString);
    ImGui::Text(ValueString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    PrintKeyword("?");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inVariableValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    PrintKeyword("@");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inConstantValueNode.ToString().c_str());

    return true;
}
#endif
