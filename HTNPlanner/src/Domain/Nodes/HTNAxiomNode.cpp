#include "Domain/Nodes/HTNAxiomNode.h"

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueNode.h"

HTNAtom HTNAxiomNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNAxiomNode::GetID() const
{
    return mIDNode->ToString();
}
