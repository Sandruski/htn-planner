#include "Domain/Nodes/HTNNodeBase.h"

HTNNodeBase::~HTNNodeBase() = default;

void HTNNodeBase::Accept(MAYBE_UNUSED const HTNNodeVisitorBase& ioNodeVisitor, MAYBE_UNUSED const HTNAtom& inNodeValue,
                         MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
}

HTNAtom HTNNodeBase::Accept(MAYBE_UNUSED const HTNNodeVisitorBase& ioNodeVisitor, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}
