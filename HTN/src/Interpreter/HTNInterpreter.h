#pragma once

#include "Interpreter/HTNDecompositionContext.h"
#include "Parser/AST/HTNNodeBase.h"
#include "Parser/AST/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNAtom;
class HTNDomainNode;
class HTNTaskInstance;
class HTNWorldState;

// Traverses an abstract syntax tree
class HTNInterpreter final : public HTNNodeVisitorBase
{
public:
    explicit HTNInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointName,
                            const HTNWorldState& inWorldState);
    ~HTNInterpreter();

    bool Interpret(std::vector<HTNTaskInstance>& outPlan);

    HTNAtom Visit(const HTNDomainNode& inDomainNode) final;
    HTNAtom Visit(const HTNConstantsNode& inConstantsNode) final;
    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode) final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode) final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode) final;
    HTNAtom Visit(const HTNConditionNodeBase& inConditionNode) final;
    HTNAtom Visit(const HTNTaskNode& inTaskNode) final;
    HTNAtom Visit(const HTNValueNode& inValueNode) final;

private:
    template<typename T>
    T EvaluateNode(const HTNNodeBase& inNode);

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointName;
    HTNDecompositionContext              mDecompositionContext;
};

template<typename T>
inline T HTNInterpreter::EvaluateNode(const HTNNodeBase& inNode)
{
    return inNode.Accept(*this).GetValue<T>();
}
