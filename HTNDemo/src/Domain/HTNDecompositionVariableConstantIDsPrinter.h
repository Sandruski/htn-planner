#pragma once

#include "Domain/HTNNodePath.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/HTNDecompositionNode.h"

#include <memory>
#include <string>
#include <vector>

class HTNDomainNode;
class HTNValueNodeBase;

/**
 * Prints the variables of a decomposition
 */
class HTNDecompositionVariableConstantIDsPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionVariableConstantIDsPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode);

    void Print();

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    bool IsArgument(const std::string& inVariableID, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments);

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNDecompositionNode&          mNode;
};
