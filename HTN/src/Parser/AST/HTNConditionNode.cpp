#include "Parser/AST/HTNConditionNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNConditionNode::HTNConditionNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                   const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNConditionNode::ToString() const
{
    std::string Description = GetIDNode()->ToString();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
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

HTNAxiomConditionNode::HTNAxiomConditionNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                                             const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes)
{
}

HTNAtom HTNAxiomConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNAxiomConditionNode::ToString() const
{
    std::string Description = GetIDNode()->ToString();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
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

HTNAndConditionNode::HTNAndConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

HTNAtom HTNAndConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNAndConditionNode::ToString() const
{
    std::string Description = "and";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

HTNOrConditionNode::HTNOrConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

HTNAtom HTNOrConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNOrConditionNode::ToString() const
{
    std::string Description = "or";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

HTNAltConditionNode::HTNAltConditionNode(const std::vector<std::shared_ptr<const HTNConditionNodeBase>>& inConditionNodes)
    : mConditionNodes(inConditionNodes)
{
}

HTNAtom HTNAltConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNAltConditionNode::ToString() const
{
    std::string Description = "alt";

    for (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode : mConditionNodes)
    {
        Description.append(std::format(" {}", ConditionNode->ToString()));
    }

    return Description;
}

HTNNotConditionNode::HTNNotConditionNode(const std::shared_ptr<const HTNConditionNodeBase>& inConditionNode) : mConditionNode(inConditionNode)
{
}

HTNAtom HTNNotConditionNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNNotConditionNode::ToString() const
{
    return std::format("not {}", mConditionNode->ToString());
}
