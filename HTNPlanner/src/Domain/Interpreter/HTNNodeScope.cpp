#include "Domain/Interpreter/HTNNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"

HTNNodeScope::HTNNodeScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID)
    : mDecompositionContext(ioDecompositionContext)
{
    mResult = mDecompositionContext.PushNodeToCurrentNodePath(inNodeID);

    // TODO salvarez Call debugger to record decomposition
}

HTNNodeScope::~HTNNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the index associated to the current node
    const std::string       CurrentNodePath      = mDecompositionContext.GetCurrentNodePath();
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    Environment.RemoveIndex(CurrentNodePath);

    // Remove the current node from the path
    mDecompositionContext.PopNodeFromCurrentNodePath();

    // TODO salvarez Call debugger to record decomposition
}
