#include "Parser/AST/HTNMethodNode.h"

#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

HTNMethodNode::HTNMethodNode(const std::shared_ptr<const HTNValueNode>&               inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueNode>>&  inArgumentNodes,
                             const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mBranchNodes(inBranchNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNMethodNode::~HTNMethodNode() = default;

std::vector<HTNTaskInstance> HTNMethodNode::Accept(const HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNMethodNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNMethodNode::ToString() const
{
    std::string Description = GetID();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
    {
        Description.append(std::format("{} ", ArgumentNode->ToString()));
    }

    if (mIsTopLevel)
    {
        Description.append("top_level_method");
    }

    return Description;
}
