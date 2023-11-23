// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Core/HTNScope.h"

#include <string>

class HTNDecompositionContext;
class HTNIndices;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the node path
 * - Removes the indices associated to the node scope when leaving it
 */
class HTNDecompositionNodeScope final : public HTNScope
{
public:
    explicit HTNDecompositionNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNDecompositionNodeScope();

private:
    HTNIndices& mIndices;
};