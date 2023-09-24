#include "HTNPrinter.h"

#include "HTNLog.h"
#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNConstantsNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"

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

HTNPrinter::HTNPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode) : mDomainNode(inDomainNode)
{
}

HTNPrinter::~HTNPrinter() = default;

bool HTNPrinter::Print()
{
    const HTNDomainNode* DomainNode = mDomainNode.get();
    if (!DomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    return GetNodeValue(*DomainNode).GetValue<bool>();
}

HTNAtom HTNPrinter::Visit(const HTNDomainNode& inDomainNode)
{
    PrintKeyword("domain");
    ImGui::SameLine();

    const std::shared_ptr<const HTNValueNode>& IDNode = inDomainNode.GetIDNode();
    GetNodeValue(*IDNode);

    if (inDomainNode.IsTopLevel())
    {
        PrintTopLevelKeyword();
    }

    ImGui::Indent();

    // TODO salvarez
    /*
    const std::vector<std::shared_ptr<const HTNConstantsNode>>& ConstantsNodes = inDomainNode.GetConstantNodes();
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantsNode : ConstantsNodes)
    {
    }
    */

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

HTNAtom HTNPrinter::Visit([[maybe_unused]] const HTNConstantsNode& inConstantsNode)
{
    PrintKeyword("constants");
    ImGui::SameLine();

    return true;
}

HTNAtom HTNPrinter::Visit([[maybe_unused]] const HTNConstantNode& inConstantNode)
{
    return true;
}

HTNAtom HTNPrinter::Visit(const HTNAxiomNode& inAxiomNode)
{
    PrintKeyword("axiom");
    ImGui::SameLine();

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomNode.GetIDNode();
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

HTNAtom HTNPrinter::Visit(const HTNMethodNode& inMethodNode)
{
    PrintKeyword("method");
    ImGui::SameLine();

    const std::shared_ptr<const HTNValueNode>& IDNode = inMethodNode.GetIDNode();
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

HTNAtom HTNPrinter::Visit(const HTNBranchNode& inBranchNode)
{
    const std::shared_ptr<const HTNValueNode>& IDNode = inBranchNode.GetIDNode();
    GetNodeValue(*IDNode);

    ImGui::Indent();

    if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = inBranchNode.GetPreConditionNode())
    {
        GetNodeValue(*PreConditionNode);
    }

    ImGui::Indent();

    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& TaskNodes = inBranchNode.GetTaskNodes();
    for (const std::shared_ptr<const HTNTaskNodeBase>& TaskNode : TaskNodes)
    {
        GetNodeValue(*TaskNode);
    }

    ImGui::Unindent();

    ImGui::Unindent();

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNConditionNode& inConditionNode)
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

HTNAtom HTNPrinter::Visit(const HTNAxiomConditionNode& inAxiomConditionNode)
{
    PrintKeyword("axiom");
    ImGui::SameLine();

    const std::shared_ptr<const HTNValueNode>& IDNode = inAxiomConditionNode.GetIDNode();
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

HTNAtom HTNPrinter::Visit(const HTNAndConditionNode& inAndConditionNode)
{
    PrintKeyword("and");
    ImGui::SameLine();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAndConditionNode.GetSubConditionNodes();
    for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
    {
        GetNodeValue(*SubConditionNode);
    }

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNOrConditionNode& inOrConditionNode)
{
    PrintKeyword("or");
    ImGui::SameLine();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inOrConditionNode.GetSubConditionNodes();
    for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
    {
        GetNodeValue(*SubConditionNode);
    }

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNAltConditionNode& inAltConditionNode)
{
    PrintKeyword("alt");
    ImGui::SameLine();

    const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& SubConditionNodes = inAltConditionNode.GetSubConditionNodes();
    for (const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode : SubConditionNodes)
    {
        GetNodeValue(*SubConditionNode);
    }

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNNotConditionNode& inNotConditionNode)
{
    PrintKeyword("not");
    ImGui::SameLine();

    const std::shared_ptr<const HTNConditionNodeBase>& SubConditionNode = inNotConditionNode.GetSubConditionNode();
    GetNodeValue(*SubConditionNode);

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNCompoundTaskNode& inCompoundTaskNode)
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

HTNAtom HTNPrinter::Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)
{
    PrintKeyword("primitive");
    ImGui::SameLine();

    const std::shared_ptr<const HTNValueNode>& IDNode = inPrimitiveTaskNode.GetIDNode();
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

HTNAtom HTNPrinter::Visit(const HTNValueNode& inValueNode)
{
    ImGui::Text(inValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    PrintKeyword("variable");
    ImGui::SameLine();

    ImGui::Text(inVariableValueNode.ToString().c_str());

    return true;
}

HTNAtom HTNPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    PrintKeyword("constant");
    ImGui::SameLine();

    ImGui::Text(inConstantValueNode.ToString().c_str());

    return true;
}
