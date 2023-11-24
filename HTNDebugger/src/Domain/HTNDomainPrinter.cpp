// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDomainPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDomainPrinterContext.h"
#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNBranchNode.h"
#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

#include "imgui.h"

namespace
{
void PrintKeyword(const std::string& inKeyword)
{
    ImGui::TextDisabled(inKeyword.c_str());
}
} // namespace

bool HTNDomainPrinter::Print(HTNDomainPrinterContext& ioDomainPrinterContext) const
{
    OPTICK_EVENT("PrintDomain");

    const std::shared_ptr<const HTNDomainNode>& DomainNode = ioDomainPrinterContext.GetDomainNode();
    if (!DomainNode)
    {
        HTN_LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode, ioDomainPrinterContext).GetValue<bool>();
}

HTNAtom HTNDomainPrinter::Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetDomainNode");

    PrintKeyword("domain");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inDomainNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    if (inDomainNode.IsTopLevel())
    {
        ImGui::SameLine();
        PrintKeyword("top_level");
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& ConstantsNodes = inDomainNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantsNode : ConstantsNodes)
    {
        GetNodeValue(*ConstantsNode, ioDomainPrinterContext);
    }

    const std::vector<std::shared_ptr<const HTNAxiomNode>>& AxiomNodes = inDomainNode.GetAxiomNodes();
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : AxiomNodes)
    {
        GetNodeValue(*AxiomNode, ioDomainPrinterContext);
    }

    const std::vector<std::shared_ptr<const HTNMethodNode>>& MethodNodes = inDomainNode.GetMethodNodes();
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : MethodNodes)
    {
        GetNodeValue(*MethodNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConstantsNode");

    PrintKeyword("constants");

    if (const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantsNode.GetIDNode())
    {
        ImGui::SameLine();
        GetNodeValue(*IDNode, ioDomainPrinterContext);
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodes = inConstantsNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodes)
    {
        GetNodeValue(*ConstantNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConstantNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantNode.GetIDNode();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    ImGui::SameLine();
    GetNodeValue(*ValueNode, ioDomainPrinterContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAxiomNode");

    PrintKeyword("axiom");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode, ioDomainPrinterContext);
    }

    if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
    {
        GetNodeValue(*ConditionNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetMethodNode");

    PrintKeyword("method");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inMethodNode.GetIDNode();
    ImGui::SameLine();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    if (inMethodNode.IsTopLevel())
    {
        ImGui::SameLine();
        PrintKeyword("top_level");
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ParameterNode, ioDomainPrinterContext);
    }

    const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = inMethodNode.GetBranchNodes();
    for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
    {
        GetNodeValue(*BranchNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetBranchNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inBranchNode.GetIDNode();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
    {
        GetNodeValue(*PreConditionNode, ioDomainPrinterContext);
    }

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
    {
        GetNodeValue(*TaskNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConditionNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConditionNode.GetIDNode();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAxiomConditionNode");

    PrintKeyword("#");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomConditionNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAndConditionNode");

    PrintKeyword("and");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDomainPrinterContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetOrConditionNode");

    PrintKeyword("or");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDomainPrinterContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAltConditionNode");

    PrintKeyword("alt");

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    if (!SubConditionNodes.empty())
    {
        ImGui::SameLine();
        const float CursorPosX = ImGui::GetCursorPosX();
        for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
        {
            GetNodeValue(*SubConditionNode, ioDomainPrinterContext);
            ImGui::NewLine();
            ImGui::SameLine(CursorPosX);
        }
        ImGui::SameLine(0.f, 0.f);
    }

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetNotConditionNode");

    PrintKeyword("not");

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    ImGui::SameLine();
    GetNodeValue(*SubConditionNode, ioDomainPrinterContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetCompoundTaskNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetPrimitiveTaskNode");

    PrintKeyword("!");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
    ImGui::SameLine(0.f, 0.f);
    GetNodeValue(*IDNode, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        GetNodeValue(*ArgumentNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNIdentifierExpressionNode&      inIdentifierExpressionNode,
                                HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetIdentifierExpressionNode");

    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     IdentifierString        = inIdentifierExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    ImGui::Text(IdentifierString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetLiteralExpressionNode");

    static constexpr bool ShouldDoubleQuoteString = true;
    const std::string     LiteralString           = inLiteralExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    ImGui::Text(LiteralString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetVariableExpressionNode");

    PrintKeyword("?");

    ImGui::SameLine(0.f, 0.f);
    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     VariableString          = inVariableExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    ImGui::Text(VariableString.c_str());

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConstantExpressionNode");

    PrintKeyword("@");

    ImGui::SameLine(0.f, 0.f);
    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     ConstantString          = inConstantExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    ImGui::Text(ConstantString.c_str());

    return true;
}
#endif
