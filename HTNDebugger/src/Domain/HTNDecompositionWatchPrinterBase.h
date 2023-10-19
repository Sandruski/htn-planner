#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionNode;
class HTNDomainNode;

class HTNDecompositionWatchPrinterBase : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionWatchPrinterBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

protected:
    const HTNDecompositionNode& mNode;

    std::vector<std::string> mNodeVariablePaths;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};
#endif
