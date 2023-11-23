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
 * Visitor pattern
 * Allows to define the behavior of an operation that can be performed on a set of types in a single place
 * Adds a layer of indirection
 * Uses function overloading
 * @see HTNNodeBase
 */
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = 0;

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual void    Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                          HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    virtual HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

protected:
    void    SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    HTNAtom GetNodeValue(const HTNNodeBase& inNode, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
};
