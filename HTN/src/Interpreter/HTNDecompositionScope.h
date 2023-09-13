#pragma once

#include <memory>

class HTNDecompositionContext;
class HTNNodeBase;

/**
 * Helper class that uses RAII (Resource Acquisition Is Initialization) to automatically push/pop a node to/from the current scope of a decomposition
 * context
 */
class HTNDecompositionScope
{
public:
    HTNDecompositionScope(HTNDecompositionContext& inDecompositionContext, const std::shared_ptr<const HTNNodeBase>& inNode);
    ~HTNDecompositionScope();

private:
    HTNDecompositionContext&           mDecompositionContext;
    std::shared_ptr<const HTNNodeBase> mNode;
};
