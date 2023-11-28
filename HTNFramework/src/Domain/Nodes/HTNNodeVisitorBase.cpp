// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include "Core/HTNAtom.h"
#include "Domain/Nodes/HTNNodeBase.h"

HTNNodeVisitorBase::~HTNNodeVisitorBase() = default;

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNDomainNode&       inDomainNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNConstantsNode&    inConstantsNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNConstantNode&     inConstantNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNAxiomNode&        inAxiomNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNMethodNode&       inMethodNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNBranchNode&       inBranchNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNConditionNode&    inConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNAxiomConditionNode& inAxiomConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&   ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNAndConditionNode& inAndConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNOrConditionNode&  inOrConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNAltConditionNode& inAltConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNNotConditionNode& inNotConditionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNCompoundTaskNode& inCompoundTaskNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNPrimitiveTaskNode& inPrimitiveTaskNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&  ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNIdentifierExpressionNode& inIdentifierExpressionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&         ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNLiteralExpressionNode& inLiteralExpressionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&      ioNodeVisitorContext) const
{
    return HTNAtom();
}

void HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNVariableExpressionNode& inVariableExpressionNode,
                               HTN_MAYBE_UNUSED const HTNAtom&                   inVariableExpressionNodeValue,
                               HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&       ioNodeVisitorContext) const
{
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNVariableExpressionNode& inVariableExpressionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&       ioNodeVisitorContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(HTN_MAYBE_UNUSED const HTNConstantExpressionNode& inConstantExpressionNode,
                                  HTN_MAYBE_UNUSED HTNNodeVisitorContextBase&       ioNodeVisitorContext) const
{
    return HTNAtom();
}

void HTNNodeVisitorBase::SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    inNode.Accept(*this, inNodeValue, ioNodeVisitorContext);
}

HTNAtom HTNNodeVisitorBase::GetNodeValue(const HTNNodeBase& inNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const
{
    return inNode.Accept(*this, ioNodeVisitorContext);
}
