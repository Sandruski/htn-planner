#pragma once

class HTNDecompositionContext;
class HTNEnvironment;

/**
 * Helper that uses RAII to automatically push/pop an environment to/from the environments of the current decomposition
 */
class HTNScope
{
public:
    HTNScope(HTNDecompositionContext& ioDecompositionContext);
    HTNScope(HTNDecompositionContext& ioDecompositionContext, const HTNEnvironment& inEnvironment);
    ~HTNScope();

private:
    HTNDecompositionContext& mDecompositionContext;
};
