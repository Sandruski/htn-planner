#include "Domain/Nodes/HTNConstantsNode.h"

#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNConstantsNode::HTNConstantsNode(const std::shared_ptr<const HTNValueNode>&                 inIDNode,
                                   const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes)
{
}

HTNConstantsNode::~HTNConstantsNode() = default;

HTNAtom HTNConstantsNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNConstantsNode::GetID() const
{
    return mIDNode->ToString();
}
