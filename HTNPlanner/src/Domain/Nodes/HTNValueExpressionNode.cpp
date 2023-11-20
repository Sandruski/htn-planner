#include "Domain/Nodes/HTNValueExpressionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNValueExpressionNodeBase::HTNValueExpressionNodeBase(const HTNAtom& inValue) : mValue(inValue)
{
}

std::string HTNValueExpressionNodeBase::ToString() const
{
    static constexpr bool ShouldDoubleQuoteString = false;
    return mValue.ToString(ShouldDoubleQuoteString);
}

HTNIdentifierExpressionNode::HTNIdentifierExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNIdentifierExpressionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNLiteralExpressionNode::HTNLiteralExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNLiteralExpressionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNVariableExpressionNode::HTNVariableExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

void HTNVariableExpressionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue,
                                       HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, inNodeValue, ioContext);
}

HTNAtom HTNVariableExpressionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

HTNConstantExpressionNode::HTNConstantExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNConstantExpressionNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}
