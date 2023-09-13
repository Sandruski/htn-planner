#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionContext;
class HTNDomainNode;
class HTNTaskInstance;

// Traverses an abstract syntax tree
class HTNInterpreter
{
public:
    explicit HTNInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode);
    ~HTNInterpreter();

    std::vector<HTNTaskInstance> Interpret(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};
