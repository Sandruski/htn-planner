// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Nodes/HTNValueExpressionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

HTNValueExpressionNodeBase::HTNValueExpressionNodeBase(const HTNAtom& inValue) : mValue(inValue)
{
}

HTNIdentifierExpressionNode::HTNIdentifierExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNIdentifierExpressionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNLiteralExpressionNode::HTNLiteralExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNLiteralExpressionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNVariableExpressionNode::HTNVariableExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

void HTNVariableExpressionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, const HTNAtom& inNodeValue,
                                       HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, inNodeValue, ioNodeVisitorContext);
}

HTNAtom HTNVariableExpressionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

HTNConstantExpressionNode::HTNConstantExpressionNode(const HTNAtom& inValue) : HTNValueExpressionNodeBase(inValue)
{
}

HTNAtom HTNConstantExpressionNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}
