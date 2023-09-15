#include "Parser/AST/HTNConstantsNode.h"

#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

HTNConstantsNode::HTNConstantsNode(const std::shared_ptr<const HTNValueNode>&                 inIDNode,
                                   const std::vector<std::shared_ptr<const HTNConstantNode>>& inConstantNodes)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes)
{
}

HTNConstantsNode::~HTNConstantsNode() = default;

HTNAtom HTNConstantsNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNConstantsNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNConstantsNode::ToString() const
{
    return GetID();
}
