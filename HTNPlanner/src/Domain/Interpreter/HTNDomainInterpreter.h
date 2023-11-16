#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

class HTNDecompositionContext;

/**
 * Decision-making
 * Returns a plan
 */
class HTNDomainInterpreter final : public HTNNodeVisitorBase
{
public:
    bool Interpret(HTNDecompositionContext& ioDecompositionContext) const;

    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
    void    Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                  HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
};
