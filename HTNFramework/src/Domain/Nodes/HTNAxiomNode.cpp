// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNAxiomNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNAxiomNode::HTNAxiomNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                           const std::shared_ptr<const HTNConditionNodeBase>&                   inConditionNode)
    : mIDNode(inIDNode), mParameterNodes(inParameterNodes), mConditionNode(inConditionNode)
{
}

HTNAtom HTNAxiomNode::Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNodeVisitor.Visit(*this, ioNodeVisitorContext);
}

std::string HTNAxiomNode::GetID() const
{
    return mIDNode->GetValue().GetValue<std::string>();
}
