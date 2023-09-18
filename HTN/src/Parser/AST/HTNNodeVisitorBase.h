#pragma once

#include "HTNAtom.h"

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNCompoundTaskNode;
class HTNConditionNodeBase;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNEnvironment;
class HTNMethodNode;
class HTNPrimitiveTaskNode;
class HTNValueNode;

// TODO salvarez Revisit this class and implement planning and printing the AST tree through it. Use templates!!!
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode)               = 0;
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode)         = 0;
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode)           = 0;
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode)                 = 0;
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode)               = 0;
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode)               = 0;
    virtual HTNAtom Visit(const HTNConditionNodeBase& inConditionNode)     = 0;
    virtual HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode)   = 0;
    virtual HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode) = 0;
    virtual HTNAtom Visit(const HTNValueNode& inValueNode)                 = 0;
};
