#include "Domain/HTNDomainPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

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

    HTNNodeVisitorContextBase Context;
    return GetNodeValue(*DomainNode, Context).GetValue<bool>();
}

HTNAtom HTNDomainPrinter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("domain");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inDomainNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioContext);

    if (inDomainNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& ConstantsNodes = inDomainNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantsNode : ConstantsNodes)
    {
        GetNodeValue(*ConstantsNode, ioContext);
    }

    const std::vector<std::shared_ptr<const HTNAxiomNode>>& AxiomNodes = inDomainNode.GetAxiomNodes();
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : AxiomNodes)
    {
        GetNodeValue(*AxiomNode, ioContext);
    }

    const std::vector<std::shared_ptr<const HTNMethodNode>>& MethodNodes = inDomainNode.GetMethodNodes();
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : MethodNodes)
    {
        GetNodeValue(*MethodNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("constants");

    if (const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantsNode.GetIDNode())
    {
        ImGui::SameLine();
        GetNodeValue(*IDNode, ioContext);
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodes = inConstantsNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodes)
    {
        GetNodeValue(*ConstantNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantNode.GetIDNode();
    GetNodeValue(*IDNode, ioContext);

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    ImGui::SameLine();
    GetNodeValue(*ValueNode, ioContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("axiom");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode, ioContext);
    }

    if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
    {
        GetNodeValue(*ConditionNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("method");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inMethodNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioContext);

    if (inMethodNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode, ioContext);
    }

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
    {
        GetNodeValue(*BranchNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inBranchNode.GetIDNode();
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
    {
        GetNodeValue(*PreConditionNode, ioContext);
    }

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
    {
        GetNodeValue(*TaskNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConditionNode.GetIDNode();
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("#");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomConditionNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("and");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("or");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("alt");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("not");

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    ImGui::SameLine();
    GetNodeValue(*SubConditionNode, ioContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext)
{
    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("!");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode, ioContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    const HTNAtom&    Value                   = inIdentifierExpressionNode.GetValue();
    constexpr bool    ShouldDoubleQuoteString = false;
    const std::string ValueString             = Value.ToString(ShouldDoubleQuoteString);
    ImGui::Text(ValueString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    const HTNAtom&    Value                   = inLiteralExpressionNode.GetValue();
    constexpr bool    ShouldDoubleQuoteString = true;
    const std::string ValueString             = Value.ToString(ShouldDoubleQuoteString);
    ImGui::Text(ValueString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("?");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inVariableExpressionNode.ToString().c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    PrintKeyword("@");

    ImGui::SameLine(0.f, 0.f);
    ImGui::Text(inConstantExpressionNode.ToString().c_str());

    return true;
}
#endif
