#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNValueNodeBase;

/**
 * Prints the variables of a decomposition
 */
class HTNDecompositionVariablesPrinter final : public HTNNodeVisitorBase
{
public:
    void Print(const HTNVariables& inVariables, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments);

    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;

private:
    bool IsArgument(const std::string& inVariableID, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments);

    HTNDecompositionContext mDecompositionContext;
};
