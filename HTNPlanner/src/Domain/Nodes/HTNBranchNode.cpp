#include "Domain/Nodes/HTNBranchNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNBranchNode::HTNBranchNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&  inIDNode,
                                    const std::shared_ptr<const HTNConditionNodeBase>&         inPreConditionNode,
                                    const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& inTaskNodes)
    : mIDNode(inIDNode), mPreConditionNode(inPreConditionNode), mTaskNodes(inTaskNodes)
{
}

HTNAtom HTNBranchNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNBranchNode::GetID() const
{
    return mIDNode->ToString();
}
