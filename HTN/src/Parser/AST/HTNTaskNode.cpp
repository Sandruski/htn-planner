#include "Parser/AST/HTNTaskNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

HTNTaskNode::HTNTaskNode(const std::shared_ptr<const HTNValueNode>& inIDNode, const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes,
                         const HTNTaskType inType)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mType(inType)
{
}

HTNTaskNode::~HTNTaskNode() = default;

std::vector<std::shared_ptr<const HTNTaskNode>> HTNTaskNode::Accept(const HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNTaskNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNTaskNode::ToString() const
{
    std::string Description = GetID();

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
