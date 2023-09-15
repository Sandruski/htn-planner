#pragma once

#include "HTNAtom.h"

#include <memory>
#include <vector>

class HTNAxiomNode;
class HTNBranchNode;
class HTNConditionNodeBase;
class HTNConstantNode;
class HTNConstantsNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNTaskNode;
class HTNValueNode;

// TODO salvarez Revisit this class and implement planning and printing the AST tree through it. Use templates!!!
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual HTNAtom Visit(const HTNDomainNode& inDomainNode)           = 0;
    virtual HTNAtom Visit(const HTNConstantsNode& inConstantsNode)     = 0;
    virtual HTNAtom Visit(const HTNConstantNode& inConstantNode)       = 0;
    virtual HTNAtom Visit(const HTNAxiomNode& inAxiomNode)             = 0;
    virtual HTNAtom Visit(const HTNMethodNode& inMethodNode)           = 0;
    virtual HTNAtom Visit(const HTNBranchNode& inBranchNode)           = 0;
    virtual HTNAtom Visit(const HTNConditionNodeBase& inConditionNode) = 0;
    virtual HTNAtom Visit(const HTNTaskNode& inTaskNode)               = 0;
    virtual HTNAtom Visit(const HTNValueNode& inValueNode)             = 0;
};
