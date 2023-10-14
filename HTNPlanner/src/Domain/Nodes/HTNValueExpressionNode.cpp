#include "Domain/Nodes/HTNValueExpressionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

std::string HTNValueExpressionNodeBase::ToString() const
{
    constexpr bool ShouldDoubleQuoteString = false;
    return mValue.ToString(ShouldDoubleQuoteString);
}

HTNAtom HTNIdentifierExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNLiteralExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

void HTNVariableExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const
{
    return ioNodeVisitor.Visit(*this, inNodeValue);
}

HTNAtom HTNVariableExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNConstantExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
