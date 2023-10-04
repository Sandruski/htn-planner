#pragma once

#include <memory>
#include <string>

class HTNDecompositionContext;

/**
 * Helper that uses RAII to automatically push/pop a node to/from the current node scope path
 */
class HTNNodeScope
{
public:
    HTNNodeScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID);
    ~HTNNodeScope();

private:
    HTNDecompositionContext& mDecompositionContext;
    bool                     mResult = false;
};
