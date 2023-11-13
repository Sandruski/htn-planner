#include "Domain/Nodes/HTNValueExpressionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNValueExpressionNodeBase::HTNValueExpressionNodeBase(const HTNAtom& inValue) : mValue(inValue)
{
}

std::string HTNValueExpressionNodeBase::ToString() const
{
    constexpr bool ShouldDoubleQuoteString = false;
    return mValue.ToString(ShouldDoubleQuoteString);
}

HTNIdentifierExpressionNode::HTNIdentifierExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNIdentifierExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNLiteralExpressionNode::HTNLiteralExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNLiteralExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNVariableExpressionNode::HTNVariableExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

void HTNVariableExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const
{
    return ioNodeVisitor.Visit(*this, inNodeValue);
}

HTNAtom HTNVariableExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNConstantExpressionNode::HTNConstantExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNConstantExpressionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
