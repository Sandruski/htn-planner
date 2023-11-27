// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

class HTNAtom;
class HTNAltConditionNode;
class HTNAndConditionNode;
class HTNAxiomNode;
class HTNAxiomConditionNode;
class HTNBranchNode;
class HTNCompoundTaskNode;
class HTNConditionNode;
class HTNConstantExpressionNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNIdentifierExpressionNode;
class HTNLiteralExpressionNode;
class HTNMethodNode;
class HTNNodeBase;
class HTNNodeVisitorContextBase;
class HTNNotConditionNode;
class HTNOrConditionNode;
class HTNPrimitiveTaskNode;
class HTNVariableExpressionNode;

/**
 * Base class for a node visitor
 * - Uses the visitor pattern, which allows to define the behavior of an operation that can be performed on a set of types in a single place
 * @see HTNNodeBase
 */
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = 0;

    // Visits the given domain node with the given context
    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given constants node with the given context
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given constant node with the given context
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given axiom node with the given context
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given method node with the given context
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given branch node with the given context
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given condition node with the given context
    virtual HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given axiom node with the given context
    virtual HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given and condition node with the given context
    virtual HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given or condition node with the given context
    virtual HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given alt condition node with the given context
    virtual HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given not condition node with the given context
    virtual HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given compound task node with the given context
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given primitive task node with the given context
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given identifier expression node with the given context
    virtual HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given literal expression node with the given context
    virtual HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given variable expression node with the given value and context
    virtual void Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                       HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given variable expression node with the given context
    virtual HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Visits the given constant expression node with the given context
    virtual HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

protected:
    // Calls the 'Accept' member method on the given node with the given value and context
    void SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Calls the 'Accept' member method on the given node with the given context
    HTNAtom GetNodeValue(const HTNNodeBase& inNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
};
