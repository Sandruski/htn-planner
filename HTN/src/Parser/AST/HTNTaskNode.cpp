#include "Parser/AST/HTNTaskNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

HTNTaskNodeBase::HTNTaskNodeBase(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                 const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNTaskNodeBase::~HTNTaskNodeBase() = default;

std::string HTNTaskNodeBase::GetID() const
{
    return mIDNode->ToString();
}

HTNCompoundTaskNode::HTNCompoundTaskNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                         const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes)
{
}

HTNAtom HTNCompoundTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNPrimitiveTaskNode::HTNPrimitiveTaskNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                           const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes)
    : HTNTaskNodeBase(inIDNode, inArgumentNodes)
{
}

HTNAtom HTNPrimitiveTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
