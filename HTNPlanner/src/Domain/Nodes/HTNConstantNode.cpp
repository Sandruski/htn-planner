#include "Domain/Nodes/HTNConstantNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNConstantNode::HTNConstantNode(const std::shared_ptr<const HTNIdentifierExpressionNode>& inIDNode,
                                        const std::shared_ptr<const HTNLiteralExpressionNode>&    inValueNode)
    : mIDNode(inIDNode), mValueNode(inValueNode)
{
}

HTNAtom HTNConstantNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNConstantNode::GetID() const
{
    return mIDNode->ToString();
}
