// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Core/HTNScope.h"
#include "HTNCoreMinimal.h"

#include <string>

class HTNDecompositionContext;
class HTNIndicesManager;

/**
 * Helper that uses RAII to automatically push/pop a node ID to/from the node path
 */
class HTNNodeScope final : public HTNScope
{
public:
    explicit HTNNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNNodeScope();

private:
    HTNIndicesManager& mIndicesManager;
};
