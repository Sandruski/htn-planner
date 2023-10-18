#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <string>
#include <vector>

class HTNDecompositionNode;

/**
 * Prints the IDs of the node's parameters or arguments
 */
class HTNDecompositionNodeParameterArgumentIDsPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionNodeParameterArgumentIDsPrinter(const HTNDecompositionNode& inNode);

    void Print();

    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

private:
    const HTNDecompositionNode& mNode;

    std::vector<std::string> mNodeVariablesPaths;
};
#endif
