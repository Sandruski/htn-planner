#pragma once

#include "HTNPlannerMinimal.h"
#include "Domain/Interpreter/HTNNodeScope.h"

#include <string>

class HTNDecompositionContext;
class HTNIndices;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the node path
 * - Removes the indices associated to the node scope when leaving it
 */
class HTNDecompositionNodeScope final : public HTNNodeScope
{
public:
    explicit HTNDecompositionNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNDecompositionNodeScope();

private:
    HTNIndices& mIndices;
};
