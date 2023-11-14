#include "Domain/Nodes/HTNAxiomNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNAxiomNode::HTNAxiomNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&            inIDNode,
                                  const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inParameterNodes,
                                  const std::shared_ptr<const HTNConditionNodeBase>&                   inConditionNode)
    : mIDNode(inIDNode), mParameterNodes(inParameterNodes), mConditionNode(inConditionNode)
{
}

HTNAtom HTNAxiomNode::Accept(HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const
{
    return ioNodeVisitor.Visit(*this, ioContext);
}

std::string HTNAxiomNode::GetID() const
{
    return mIDNode->ToString();
}
