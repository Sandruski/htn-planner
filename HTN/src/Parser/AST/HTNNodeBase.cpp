#include "Parser/AST/HTNNodeBase.h"

void HTNNodeBase::Accept([[maybe_unused]] HTNNodeVisitorBase& ioNodeVisitor, [[maybe_unused]] const HTNAtom& inNodeValue) const
{
}

HTNAtom HTNNodeBase::Accept([[maybe_unused]] HTNNodeVisitorBase& ioNodeVisitor) const
{
    return HTNAtom();
}
