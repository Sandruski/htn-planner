#include "Domain/Nodes/HTNBranchNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNAtom HTNBranchNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNBranchNode::GetID() const
{
    return mIDNode->ToString();
}
