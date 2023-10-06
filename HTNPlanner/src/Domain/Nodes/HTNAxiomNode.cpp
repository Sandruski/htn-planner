#include "Domain/Nodes/HTNAxiomNode.h"

#include "Domain/Nodes/HTNConditionNode.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNAxiomNode::HTNAxiomNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                           const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                           const std::shared_ptr<const HTNConditionNodeBase>&      inConditionNode)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mConditionNode(inConditionNode)
{
}

HTNAxiomNode::~HTNAxiomNode() = default;

HTNAtom HTNAxiomNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNAxiomNode::GetID() const
{
    return mIDNode->ToString();
}
