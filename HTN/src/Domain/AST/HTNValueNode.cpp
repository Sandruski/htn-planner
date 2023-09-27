#include "Domain/AST/HTNValueNode.h"

#include "Domain/AST/HTNNodeVisitorBase.h"

std::string HTNValueNodeBase::ToString() const
{
    return mValue.ToString();
}

HTNAtom HTNValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

void HTNVariableValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const
{
    return ioNodeVisitor.Visit(*this, inNodeValue);
}

HTNAtom HTNVariableValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNConstantValueNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
