#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionNode;
class HTNDomainNode;

/**
 * Prints the watch window for the selected node
 */
class HTNDecompositionWatchWindowPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionWatchWindowPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    void Print();

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNDecompositionNode&          mNode;

    std::vector<std::string> mNodeVariablePaths;
};
#endif
