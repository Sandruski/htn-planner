#include "Domain/HTNNodeScope.h"

#include "Domain/HTNNodePath.h"
//#include "Domain/HTNEnvironment.h"

HTNNodeScope::HTNNodeScope(HTNNodePath& outNodePath, const std::string& inNodeID) : mNodePath(outNodePath)
{
    mResult = mNodePath.TryPushNodeToNodePath(inNodeID);
    if (!mResult)
    {
        return;
    }

#ifdef HTN_DEBUG
    //const std::string CurrentNodePath = mDecompositionContext.GetCurrentNodePath();
    //ioDecompositionContext.RecordNodeSnapshot(CurrentNodePath);
#endif
}

HTNNodeScope::~HTNNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the index associated to the current node
    //const std::string       CurrentNodePath      = mDecompositionContext.GetCurrentNodePath();
    //HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    //HTNEnvironment&         Environment          = CurrentDecomposition.GetEnvironmentMutable();
    //Environment.RemoveIndex(CurrentNodePath);

    // Remove the current node from the path
    const bool Result = mNodePath.TryPopNodeFromNodePath();
    assert(Result);

    // TODO salvarez Call debugger to record decomposition
}
