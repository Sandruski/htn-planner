#include "Domain/Nodes/HTNTaskNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNTaskNodeBase::~HTNTaskNodeBase() = default;

std::string HTNTaskNodeBase::GetID() const
{
    return mIDNode->ToString();
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const bool inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNCompoundTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

const std::string& HTNCompoundTaskNode::GetMethodNodeID() const
{
    return mIDNode->GetValue().GetValue<std::string>();
}

HTNPrimitiveTaskNode::HTNPrimitiveTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                           const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes)
{
}

HTNAtom HTNPrimitiveTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
