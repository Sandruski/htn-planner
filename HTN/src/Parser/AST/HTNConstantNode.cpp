#include "Parser/AST/HTNConstantNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

HTNConstantNode::HTNConstantNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::shared_ptr<const HTNValueNodeBase>& inArgumentNode)
    : mIDNode(inIDNode), mArgumentNode(inArgumentNode)
{
}

HTNConstantNode::~HTNConstantNode() = default;

HTNAtom HTNConstantNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNConstantNode::GetID() const
{
    return mIDNode->ToString();
}
