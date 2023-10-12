#pragma once

#include "Domain/HTNNodeScope.h"

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
    HTNDecompositionNodeScope(HTNDecompositionContext& outDecompositionContext, const std::string& inNodeID);
    ~HTNDecompositionNodeScope();

private:
    HTNIndices& mIndices;
};