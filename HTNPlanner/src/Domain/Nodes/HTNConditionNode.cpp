#include "Domain/Nodes/HTNConditionNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"

std::string HTNConditionNodeBase::GetID() const
{
    return std::to_string(mID);
}

HTNAtom HTNConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNAxiomConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNAndConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNOrConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNAltConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

HTNAtom HTNNotConditionNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}
