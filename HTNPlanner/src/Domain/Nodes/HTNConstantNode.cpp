#include "Domain/Nodes/HTNConstantNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNAtom HTNConstantNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNConstantNode::GetID() const
{
    return mIDNode->ToString();
}
