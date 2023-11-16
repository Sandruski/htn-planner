#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNNodeScope.h"

#include <string>

class HTNDecompositionContext;
class HTNVariables;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the variable scope path
 * - Removes the variables associated to the node scope when leaving it
 */
class HTNDecompositionVariableScopeNodeScope final : public HTNNodeScope
{
public:
    explicit HTNDecompositionVariableScopeNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNDecompositionVariableScopeNodeScope();

private:
    HTNVariables& mVariables;
};
