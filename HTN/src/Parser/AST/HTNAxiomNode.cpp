#include "Parser/AST/HTNAxiomNode.h"

#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

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
