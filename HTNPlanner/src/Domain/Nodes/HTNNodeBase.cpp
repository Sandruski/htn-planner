#include "Domain/Nodes/HTNNodeBase.h"

void HTNNodeBase::Accept(MAYBE_UNUSED HTNNodeVisitorBase& ioNodeVisitor, MAYBE_UNUSED const HTNAtom& inNodeValue) const
{
}

HTNAtom HTNNodeBase::Accept(MAYBE_UNUSED HTNNodeVisitorBase& ioNodeVisitor) const
{
    return HTNAtom();
}
