#pragma once

#include "HTNAtom.h"

class HTNAltConditionNode;
class HTNAndConditionNode;
class HTNAxiomNode;
class HTNAxiomConditionNode;
class HTNBranchNode;
class HTNCompoundTaskNode;
class HTNConditionNode;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNEnvironment;
class HTNMethodNode;
class HTNNotConditionNode;
class HTNOrConditionNode;
class HTNPrimitiveTaskNode;
class HTNValueNode;

// TODO salvarez Revisit this class and implement planning and printing the AST tree through it. Use templates!!!
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode)                 = 0;
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode)           = 0;
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode)             = 0;
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode)                   = 0;
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode)                 = 0;
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode)                 = 0;
    virtual HTNAtom Visit(const HTNConditionNode& inConditionNode)           = 0;
    virtual HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode) = 0;
    virtual HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode)     = 0;
    virtual HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode)       = 0;
    virtual HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode)     = 0;
    virtual HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode)     = 0;
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode)     = 0;
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode)   = 0;
    virtual HTNAtom Visit(const HTNValueNode& inValueNode)                   = 0;
};
