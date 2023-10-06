#include "Domain/Nodes/HTNConditionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNConditionNode::HTNConditionNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAxiomConditionNode::HTNAxiomConditionNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                             const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNAxiomConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

const std::string& HTNAxiomConditionNode::GetAxiomNodeID() const
{
    return mIDNode->GetValue().GetValue<std::string>();
}

HTNAndConditionNode::HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAndConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNOrConditionNode::HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNOrConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAltConditionNode::HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inSubConditionNodes)
    : mSubConditionNodes(inSubConditionNodes)
{
}

HTNAtom HTNAltConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNNotConditionNode::HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inSubConditionNode)
    : mSubConditionNode(inSubConditionNode)
{
}

HTNAtom HTNNotConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
