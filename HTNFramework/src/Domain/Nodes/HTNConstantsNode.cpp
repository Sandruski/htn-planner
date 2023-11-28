// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Nodes/HTNConstantsNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNConstantsNode::HTNConstantsNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                                   const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes)
{
}

HTNAtom HTNConstantsNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

std::string HTNConstantsNode::GetID() const
{
    return mIDNode->GetValue().GetValue<std::string>();
}
