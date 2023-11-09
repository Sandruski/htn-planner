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
    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode) final;

protected:
    void Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    const HTNDecompositionNode* mNode = nullptr;

    std::vector<std::string> mNodeVariablePaths;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};
#endif
