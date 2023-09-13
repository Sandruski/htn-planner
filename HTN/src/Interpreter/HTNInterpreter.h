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

    std::vector<HTNTaskInstance> Visit(const HTNDomainNode& inDomainNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNConstantsNode& inConstantsNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNConstantNode& inConstantNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNAxiomNode& inAxiomNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNMethodNode& inMethodNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNBranchNode& inBranchNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNConditionNodeBase& inConditionNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNTaskNode& inTaskNode) const final;
    std::vector<HTNTaskInstance> Visit(const HTNValueNode& inValueNode) const final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};
