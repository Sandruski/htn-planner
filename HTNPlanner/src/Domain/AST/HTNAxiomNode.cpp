#include "Domain/AST/HTNAxiomNode.h"

#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNNodeVisitorBase.h"
#include "Domain/AST/HTNValueNode.h"

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
