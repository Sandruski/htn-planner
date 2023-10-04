#include "Domain/AST/HTNMethodNode.h"

#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNNodeVisitorBase.h"
#include "Domain/AST/HTNValueNode.h"

HTNMethodNode::HTNMethodNode(const std::shared_ptr<const HTNValueNode>&               inIDNode,
                             const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArgumentNodes,
                             const std::vector<std::shared_ptr<const HTNBranchNode>>& inBranchNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mBranchNodes(inBranchNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNMethodNode::~HTNMethodNode() = default;

HTNAtom HTNMethodNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNMethodNode::GetID() const
{
    return mIDNode->ToString();
}
