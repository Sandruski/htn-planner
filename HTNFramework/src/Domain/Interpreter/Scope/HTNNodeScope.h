// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNScope.h"
#include "HTNCoreMinimal.h"

#include <string>

class HTNDecompositionContext;
class HTNIndicesManager;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the node path
 * - Removes the indices associated to the node scope when leaving it
 */
class HTNNodeScope final : public HTNScope
{
public:
    explicit HTNNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNNodeScope();

private:
    HTNIndicesManager& mIndicesManager;
};
