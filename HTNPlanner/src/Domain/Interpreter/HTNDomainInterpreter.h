#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>

class HTNDecompositionContext;
class HTNDomainNode;
enum class HTNNodeStep : unsigned char;

// Decision-making
// Returns a plan from an abstract syntax tree
class HTNDomainInterpreter final : public HTNNodeVisitorBase
{
public:
    bool Interpret(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
                   HTNDecompositionContext& ioDecompositionContext);

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
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    void    Visit(const HTNVariableExpressionNode& inVariableExpressionNode, const HTNAtom& inVariableExpressionNodeValue) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

private:
    void Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const std::string& inEntryPointID,
               HTNDecompositionContext& ioDecompositionContext);

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;
    HTNDecompositionContext*             mDecompositionContext = nullptr;

#ifdef HTN_DEBUG
private:
    void RecordCurrentNodeSnapshot(const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
    void RecordCurrentNodeSnapshot(const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
#endif
};
