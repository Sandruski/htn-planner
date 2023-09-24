#pragma once

class HTNAtom;
class HTNAltConditionNode;
class HTNAndConditionNode;
class HTNAxiomNode;
class HTNAxiomConditionNode;
class HTNBranchNode;
class HTNCompoundTaskNode;
class HTNConditionNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNConstantValueNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNNodeBase;
class HTNNotConditionNode;
class HTNOrConditionNode;
class HTNPrimitiveTaskNode;
class HTNValueNode;
class HTNVariableValueNode;

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
    virtual HTNAtom Visit(const HTNValueNode& inValueNode);
    virtual void    Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue);
    virtual HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode);
    virtual HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode);

protected:
    void    SetNodeValue(const HTNNodeBase& inNode, const HTNAtom& inNodeValue);
    HTNAtom GetNodeValue(const HTNNodeBase& inNode);
};
