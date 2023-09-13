#pragma once

#include "Interpreter/HTNTaskInstance.h" // TODO salvarez Remove this once return type of Visit is changed

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

    virtual std::vector<HTNTaskInstance> Visit(const HTNDomainNode& inDomainNode) const           = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNConstantsNode& inConstantsNode) const     = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNConstantNode& inConstantNode) const       = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNAxiomNode& inAxiomNode) const             = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNMethodNode& inMethodNode) const           = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNBranchNode& inBranchNode) const           = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNConditionNodeBase& inConditionNode) const = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNTaskNode& inTaskNode) const               = 0;
    virtual std::vector<HTNTaskInstance> Visit(const HTNValueNode& inValueNode) const             = 0;
};
