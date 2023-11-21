// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNConstantsNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNConstantsNode::HTNConstantsNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                                   const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes)
{
}

HTNAtom HTNConstantsNode::Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

std::string HTNConstantsNode::GetID() const
{
    return mIDNode->ToString();
}
