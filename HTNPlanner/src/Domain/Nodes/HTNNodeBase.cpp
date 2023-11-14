#include "Domain/Nodes/HTNNodeBase.h"

void HTNNodeBase::Accept(MAYBE_UNUSED HTNNodeVisitorBase& ioNodeVisitor, MAYBE_UNUSED const HTNAtom& inNodeValue,
                         MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
}

HTNAtom HTNNodeBase::Accept(MAYBE_UNUSED HTNNodeVisitorBase& ioNodeVisitor, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}
