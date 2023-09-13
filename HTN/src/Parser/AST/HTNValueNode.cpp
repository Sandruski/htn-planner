#include "Parser/AST/HTNValueNode.h"

#include "Parser/AST/HTNNodeVisitorBase.h"

std::vector<HTNTaskInstance> HTNValueNode::Accept(const HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNValueNode::GetID() const
{
    // Not used
    return "";
}

std::string HTNValueNode::ToString() const
{
    return mValue.ToString();
}
