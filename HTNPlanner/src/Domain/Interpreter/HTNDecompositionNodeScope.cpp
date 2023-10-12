#include "Domain/Interpreter/HTNDecompositionNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNIndices.h"

HTNDecompositionNodeScope::HTNDecompositionNodeScope(HTNDecompositionContext& outDecompositionContext, const std::string& inNodeID)
    : HTNNodeScope(outDecompositionContext.GetCurrentNodePathMutable(), inNodeID),
      mIndices(outDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetIndicesMutable())
{
    if (!mResult)
    {
        return;
    }

#ifdef HTN_DEBUG
    const std::string& NodePath = mNodePath.GetNodePath();
    outDecompositionContext.RecordNodeSnapshot(NodePath);
#endif
}

HTNDecompositionNodeScope::~HTNDecompositionNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the index associated to the node path
    const std::string& NodePath = mNodePath.GetNodePath();
    mIndices.RemoveIndex(NodePath);
}