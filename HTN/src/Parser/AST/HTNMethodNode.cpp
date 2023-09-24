#include "Parser/AST/HTNMethodNode.h"

#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

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
