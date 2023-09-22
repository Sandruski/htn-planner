#include "Parser/AST/HTNValueNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"

std::string HTNValueNodeBase::ToString() const
{
    return mValue.ToString();
}

HTNAtom HTNValueNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

HTNAtom HTNVariableValueNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

HTNAtom HTNConstantValueNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}
