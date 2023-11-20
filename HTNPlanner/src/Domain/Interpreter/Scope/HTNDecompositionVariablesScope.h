#pragma once

#include "HTNPlannerMinimal.h"
#include "HTNScope.h"

#include <string>

class HTNDecompositionContext;
class HTNVariables;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the variable scope path
 * - Removes the variables associated to the node scope when leaving it
 */
class HTNDecompositionVariablesScope final : public HTNScope
{
public:
    explicit HTNDecompositionVariablesScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNDecompositionVariablesScope();

private:
    HTNVariables& mVariables;
};
