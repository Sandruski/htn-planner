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

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual void    Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue,
                          HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) const;

protected:
    void    SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioContext) const;
    HTNAtom GetNodeValue(const HTNNodeBase& inNode, HTNNodeVisitorContextBase& ioContext) const;
};
