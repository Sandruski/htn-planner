#pragma once

#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>

class HTNDecompositionContext;
class HTNDomainNode;

// Decision-making
// Returns a plan from an abstract syntax tree
class HTNDomainInterpreter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDomainInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                  HTNDecompositionContext& ioDecompositionContext);

    bool Interpret();

    HTNAtom Visit(const HTNDomainNode& inDomainNode) final;
    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode) final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode) final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode) final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode) final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode) final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode) final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode) final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode) final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode) final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode) final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode) final;
    HTNAtom Visit(const HTNValueNode& inValueNode) final;
    void    Visit(const HTNVariableValueNode& inVariableValueNode, const HTNAtom& inVariableValueNodeValue) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    HTNDecompositionContext&             mDecompositionContext;

#ifdef HTN_DEBUG
private:
    void RecordCurrentNodeSnapshot(const bool inResult);
#endif
};

inline HTNDomainInterpreter::HTNDomainInterpreter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                                                  HTNDecompositionContext& ioDecompositionContext)
    : mDomainNode(inDomainNode), mEntryPointID(inEntryPointID), mDecompositionContext(ioDecompositionContext)
{
}
