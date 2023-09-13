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
    ~HTNInterpreter();

    bool Init(const std::string& inDomainPath);

    std::vector<HTNTaskInstance> Interpret(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const;

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNInterpreter::GetDomainNode() const
{
    return mDomainNode;
}
