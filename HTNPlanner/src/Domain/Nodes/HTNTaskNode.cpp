#include "Domain/Nodes/HTNTaskNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

std::string HTNTaskNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNAtom HTNCompoundTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNPrimitiveTaskNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
