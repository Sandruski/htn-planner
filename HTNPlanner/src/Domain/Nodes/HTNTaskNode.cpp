#include "Domain/Nodes/HTNTaskNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mID(GenerateID())
{
}

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const unsigned int inID)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mID(inID)
{
}

HTNTaskNodeBase::~HTNTaskNodeBase() = default;

std::string HTNTaskNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const bool inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes, const unsigned int inID,
                                         const bool inIsTopLevel)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes, inID), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNCompoundTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
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
