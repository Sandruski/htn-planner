#pragma once

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
    explicit HTNDecompositionVariableScopeNodeScope(HTNDecompositionContext& outDecompositionContext, const std::string& inNodeID);
    ~HTNDecompositionVariableScopeNodeScope();

private:
    HTNVariables& mVariables;
};
