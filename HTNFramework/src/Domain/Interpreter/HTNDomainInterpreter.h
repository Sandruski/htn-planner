// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "HTNCoreMinimal.h"

class HTNDecompositionContext;

/**
 * Decomposes an entry point of a domain using a world state
 * Returns a plan
 */
class HTNDomainInterpreter final : public HTNNodeVisitorBase
{
public:
    // Main decomposition member method
    bool Interpret(HTNDecompositionContext& ioDecompositionContext) const;

    // Visits the given domain node with the given decomposition context
    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given constant node with the given decomposition context
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given axiom node with the given decomposition context
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given method node with the given decomposition context
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given branch node with the given decomposition context
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given condition node with the given decomposition context
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given axiom condition node with the given decomposition context
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given and condition node with the given decomposition context
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given or condition node with the given decomposition context
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given alt condition node with the given decomposition context
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given not condition node with the given decomposition context
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given compound task node with the given decomposition context
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given primitive task node with the given decomposition context
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given identifier expression node with the given decomposition context
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given literal expression node with the given decomposition context
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given variable expression node with the given value and decomposition context
    void Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
               HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given variable expression node with the given decomposition context
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;

    // Visits the given constant expression node with the given decomposition context
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioDecompositionContext) const final;
};
