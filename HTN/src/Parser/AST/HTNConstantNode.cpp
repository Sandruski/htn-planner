#include "Parser/AST/HTNConstantNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

HTNConstantNode::HTNConstantNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNValueNodeBase>& inArgumentNode)
    : mIDNode(inIDNode), mArgumentNode(inArgumentNode)
{
}

HTNConstantNode::~HTNConstantNode() = default;

HTNAtom HTNConstantNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNConstantNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNConstantNode::ToString() const
{
    return std::format("{} {}", GetID(), mArgumentNode->ToString());
}
