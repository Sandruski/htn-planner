// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNNodeBase.h"

HTNNodeBase::~HTNNodeBase() = default;

void HTNNodeBase::Accept(HTN_MAYBE_UNUSED const HTNNodeVisitorBase& ioNodeVisitor, HTN_MAYBE_UNUSED const HTNAtom& inNodeValue,
                         HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
}

HTNAtom HTNNodeBase::Accept(HTN_MAYBE_UNUSED const HTNNodeVisitorBase& ioNodeVisitor, HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}
