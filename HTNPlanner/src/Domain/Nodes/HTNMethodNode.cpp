#include "Domain/Nodes/HTNMethodNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNAtom HTNMethodNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNMethodNode::GetID() const
{
    return mIDNode->ToString();
}
