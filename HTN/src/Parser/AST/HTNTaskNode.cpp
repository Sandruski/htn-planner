#include "Parser/AST/HTNTaskNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

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

std::string HTNTaskNodeBase::ToString() const
{
    std::string Description = GetID();

    for (const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode : mArgumentNodes)
    {
        Description.append(std::format("{} ", ArgumentNode->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
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
