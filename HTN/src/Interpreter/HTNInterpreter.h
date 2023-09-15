#pragma once

#include "Parser/AST/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionContext;
class HTNDomainNode;
class HTNTaskInstance;

// Traverses an abstract syntax tree
class HTNInterpreter final : public HTNNodeVisitorBase
{
public:
    explicit HTNInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode);
    ~HTNInterpreter();

    std::vector<HTNTaskInstance> Interpret(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const;

    void Visit(const HTNDomainNode& inDomainNode) const final;
    void Visit(const HTNConstantsNode& inConstantsNode) const final;
    void Visit(const HTNConstantNode& inConstantNode) const final;
    void Visit(const HTNAxiomNode& inAxiomNode) const final;
    void Visit(const HTNMethodNode& inMethodNode) const final;
    void Visit(const HTNBranchNode& inBranchNode) const final;
    void Visit(const HTNConditionNodeBase& inConditionNode) const final;
    void Visit(const HTNTaskNode& inTaskNode) const final;
    void Visit(const HTNValueNode& inValueNode) const final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};
