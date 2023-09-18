#pragma once

class HTNDecompositionContext;

/**
 * Helper class that uses RAII (Resource Acquisition Is Initialization) to automatically push/pop a node to/from the current scope of a decomposition
 * context
 */
class HTNDecompositionScope
{
public:
    HTNDecompositionScope(HTNDecompositionContext& ioDecompositionContext);
    ~HTNDecompositionScope();

private:
    HTNDecompositionContext& mDecompositionContext;
};
