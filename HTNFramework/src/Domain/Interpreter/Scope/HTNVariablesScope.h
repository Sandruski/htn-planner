// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNScope.h"
#include "HTNCoreMinimal.h"

#include <string>

class HTNDecompositionContext;
class HTNVariablesManager;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the variable scope path
 * - Removes the variables associated to the node scope when leaving it
 */
class HTNVariablesScope final : public HTNScope
{
public:
    explicit HTNVariablesScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext);
    ~HTNVariablesScope();

private:
    HTNVariablesManager& mVariablesManager;
};
