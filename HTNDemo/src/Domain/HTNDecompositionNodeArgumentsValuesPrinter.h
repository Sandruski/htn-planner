#pragma once

#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <vector>
#include <string>

class HTNDecompositionNode;
class HTNDomainNode;

/**
 * Prints the values of the node arguments
 */
class HTNDecompositionNodeArgumentsValuesPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionNodeArgumentsValuesPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    void Print();

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNValueNode& inValueNode) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNDecompositionNode&          mNode;

    std::vector<std::string> mNodeVariablesPaths;
};
