// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include "Domain/Nodes/HTNNodeBase.h"
#include "Helpers/HTNAtom.h"

HTNNodeVisitorBase::~HTNNodeVisitorBase() = default;

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNDomainNode& inDomainNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantsNode& inConstantsNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantNode& inConstantNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAxiomNode& inAxiomNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNMethodNode& inMethodNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNBranchNode& inBranchNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConditionNode& inConditionNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAxiomConditionNode& inAxiomConditionNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&   ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAndConditionNode& inAndConditionNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNOrConditionNode& inOrConditionNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNAltConditionNode& inAltConditionNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNNotConditionNode& inNotConditionNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNCompoundTaskNode& inCompoundTaskNode, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNPrimitiveTaskNode& inPrimitiveTaskNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&  ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNIdentifierExpressionNode& inIdentifierExpressionNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&         ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNLiteralExpressionNode& inLiteralExpressionNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&      ioContext) const
{
    return HTNAtom();
}

void HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNVariableExpressionNode& inVariableExpressionNode,
                               MAYBE_UNUSED const HTNAtom& inVariableExpressionNodeValue, MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
{
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNVariableExpressionNode& inVariableExpressionNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&       ioContext) const
{
    return HTNAtom();
}

HTNAtom HTNNodeVisitorBase::Visit(MAYBE_UNUSED const HTNConstantExpressionNode& inConstantExpressionNode,
                                  MAYBE_UNUSED HTNNodeVisitorContextBase&       ioContext) const
{
    return HTNAtom();
}

void HTNNodeVisitorBase::SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioContext) const
{
    inNode.Accept(*this, inNodeValue, ioContext);
}

HTNAtom HTNNodeVisitorBase::GetNodeValue(const HTNNodeBase& inNode, HTNNodeVisitorContextBase& ioContext) const
{
    return inNode.Accept(*this, ioContext);
}
