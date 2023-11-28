// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Nodes/HTNNodeBase.h"

HTNNodeBase::~HTNNodeBase() = default;

void HTNNodeBase::Accept(HTN_MAYBE_UNUSED const HTNNodeVisitorBase& inNodeVisitor, HTN_MAYBE_UNUSED const HTNAtom& inNodeValue,
                         HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
}

HTNAtom HTNNodeBase::Accept(HTN_MAYBE_UNUSED const HTNNodeVisitorBase&  inNodeVisitor,
                            HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}
