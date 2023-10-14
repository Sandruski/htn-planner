#pragma once

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
class HTNNotConditionNode;
class HTNOrConditionNode;
class HTNPrimitiveTaskNode;
class HTNVariableExpressionNode;

class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode);
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode);
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode);
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode);
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode);
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode);
    virtual HTNAtom Visit(const HTNConditionNode& inConditionNode);
    virtual HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode);
    virtual HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode);
    virtual HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode);
    virtual HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode);
    virtual HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode);
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode);
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode);
    virtual HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode);
    virtual HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode);
    virtual void    Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue);
    virtual HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode);
    virtual HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode);

protected:
    void    SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue);
    HTNAtom GetNodeValue(const HTNNodeBase& inNode);
};
