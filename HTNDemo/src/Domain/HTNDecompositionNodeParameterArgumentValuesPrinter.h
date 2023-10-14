#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <vector>
#include <string>

class HTNDecompositionNode;
class HTNDomainNode;

/**
 * Prints the values of the node's parameters or arguments
 */
class HTNDecompositionNodeParameterArgumentValuesPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionNodeParameterArgumentValuesPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                const HTNDecompositionNode&                 inNode);

    void Print();

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNDecompositionNode&          mNode;

    std::vector<std::string> mNodeVariablesPaths;
};
#endif
