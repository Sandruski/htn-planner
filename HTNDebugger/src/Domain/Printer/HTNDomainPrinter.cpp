// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDomainPrinter.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Printer/HTNDomainPrinterContext.h"
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

    const std::string IDString = "domain";
    PrintKeyword(IDString);

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inDomainNode.GetIDNode();
    static const std::string                                  Prefix = "";
    ImGui::SameLine();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    if (inDomainNode.IsTopLevel())
    {
        ImGui::SameLine();
        PrintKeyword(HTNDomainHelpers::kTopLevelDomainID);
    }

    ImGui::NewLine();

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

    const std::string IDString = "constants";
    PrintKeyword(IDString);

    if (const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantsNode.GetIDNode())
    {
        static const std::string Prefix = "";
        ImGui::SameLine();
        PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);
    }

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodes = inConstantsNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodes)
    {
        GetNodeValue(*ConstantNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    ImGui::NewLine();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConstantNode");

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConstantNode.GetIDNode();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    ImGui::SameLine();
    PrintValueExpressionNode(ValueNode, Prefix, ioDomainPrinterContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAxiomNode");

    const std::string IDString = "axiom";
    PrintKeyword(IDString);

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inAxiomNode.GetIDNode();
    ImGui::SameLine();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inAxiomNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ParameterNode, Prefix, ioDomainPrinterContext);
    }

    if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = inAxiomNode.GetConditionNode())
    {
        GetNodeValue(*ConditionNode, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    ImGui::NewLine();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetMethodNode");

    const std::string IDString = "method";
    PrintKeyword(IDString);

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inMethodNode.GetIDNode();
    ImGui::SameLine();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    if (inMethodNode.IsTopLevel())
    {
        ImGui::SameLine();
        PrintKeyword(HTNDomainHelpers::kTopLevelMethodID);
    }

    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& ParameterNodes = inMethodNode.GetParameterNodes();
    for (const std::shared_ptr<const HTNVariableExpressionNode>& ParameterNode : ParameterNodes)
    {
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ParameterNode, Prefix, ioDomainPrinterContext);
    }

    ImGui::NewLine();

    ImGui::Indent();
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

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inBranchNode.GetIDNode();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

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

    ImGui::NewLine();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConditionNode");

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inConditionNode.GetIDNode();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ArgumentNode, Prefix, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAxiomConditionNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode   = inAxiomConditionNode.GetIDNode();
    static const std::string                                  IDPrefix = "#";
    PrintValueExpressionNode(IDNode, IDPrefix, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inAxiomConditionNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        static const std::string ArgumentPrefix = "";
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ArgumentNode, ArgumentPrefix, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetAndConditionNode");

    const std::string IDString = "and";
    PrintKeyword(IDString);

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

    const std::string IDString = "or";
    PrintKeyword(IDString);

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

    const std::string IDString = "alt";
    PrintKeyword(IDString);

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

    const std::string IDString = "not";
    PrintKeyword(IDString);

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    ImGui::SameLine();
    GetNodeValue(*SubConditionNode, ioDomainPrinterContext);

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetCompoundTaskNode");

    static const std::string Prefix = "";

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode = inCompoundTaskNode.GetIDNode();
    PrintValueExpressionNode(IDNode, Prefix, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inCompoundTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ArgumentNode, Prefix, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}

HTNAtom HTNDomainPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetPrimitiveTaskNode");

    const std::shared_ptr<const HTNIdentifierExpressionNode>& IDNode   = inPrimitiveTaskNode.GetIDNode();
    static const std::string                                  IDPrefix = "!";
    PrintValueExpressionNode(IDNode, IDPrefix, ioDomainPrinterContext);

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& ArgumentNodes = inPrimitiveTaskNode.GetArgumentNodes();
    for (const std::shared_ptr<const HTNValueExpressionNodeBase>& ArgumentNode : ArgumentNodes)
    {
        static const std::string ArgumentPrefix = "";
        ImGui::SameLine();
        PrintColoredValueExpressionNode(ArgumentNode, ArgumentPrefix, ioDomainPrinterContext);
    }
    ImGui::Unindent();

    return true;
}
#endif
