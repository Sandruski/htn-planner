// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNConstantNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNConstantNode::HTNConstantNode(const std::shared_ptr<const HTNIdentifierExpressionNode>& inIDNode,
                                 const std::shared_ptr<const HTNLiteralExpressionNode>&    inValueNode)
    : mIDNode(inIDNode), mValueNode(inValueNode)
{
}

HTNAtom HTNConstantNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

std::string HTNConstantNode::GetID() const
{
    return mIDNode->GetValue().GetValue<std::string>();
}
