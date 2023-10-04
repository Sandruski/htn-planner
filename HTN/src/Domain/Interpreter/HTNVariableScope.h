#pragma once

#include <memory>
#include <string>

class HTNDecompositionContext;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the current variable scope path
 */
class HTNVariableScope
{
public:
    HTNVariableScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID);
    ~HTNVariableScope();

private:
    HTNDecompositionContext& mDecompositionContext;
    bool                     mResult = false;
};
