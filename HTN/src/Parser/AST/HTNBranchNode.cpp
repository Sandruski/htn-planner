#include "Parser/AST/HTNBranchNode.h"

#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"

HTNBranchNode::HTNBranchNode(const std::shared_ptr<const HTNValueNode>&                 inIDNode,
                             const std::shared_ptr<const HTNConditionNodeBase>&         inPreConditionNode,
                             const std::vector<std::shared_ptr<const HTNTaskNodeBase>>& inTaskNodes)
    : mIDNode(inIDNode), mPreConditionNode(inPreConditionNode), mTaskNodes(inTaskNodes)
{
}

HTNBranchNode::~HTNBranchNode() = default;

HTNAtom HTNBranchNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNBranchNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNBranchNode::ToString() const
{
    return GetID();
}
