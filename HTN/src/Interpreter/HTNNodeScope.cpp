#include "Interpreter/HTNNodeScope.h"

#include "Interpreter/HTNDecompositionContext.h"

HTNNodeScope::HTNNodeScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID)
    : mDecompositionContext(ioDecompositionContext)
{
    mResult = mDecompositionContext.PushNodeToCurrentNodePath(inNodeID);
}

HTNNodeScope::~HTNNodeScope()
{
    if (mResult)
    {
        mDecompositionContext.PopNodeFromCurrentNodePath();
    }
}
