#include "Domain/Nodes/HTNConstantsNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNAtom HTNConstantsNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNConstantsNode::GetID() const
{
    return mIDNode->ToString();
}
