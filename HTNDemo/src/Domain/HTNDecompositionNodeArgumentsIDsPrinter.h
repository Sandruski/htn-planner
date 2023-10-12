#pragma once

#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <string>
#include <vector>

class HTNDecompositionNode;

/**
 * Prints the IDs of the node arguments
 */
class HTNDecompositionNodeArgumentsIDsPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionNodeArgumentsIDsPrinter(const HTNDecompositionNode& inNode);

    void Print();

    HTNAtom Visit(const HTNValueNode& inValueNode) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    const HTNDecompositionNode& mNode;

    std::vector<std::string> mNodeVariablesPaths;
};
