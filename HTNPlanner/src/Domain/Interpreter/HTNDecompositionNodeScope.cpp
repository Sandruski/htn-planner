#include "Domain/Interpreter/HTNDecompositionNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNIndices.h"

HTNDecompositionNodeScope::HTNDecompositionNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext)
    : HTNNodeScope(inNodeID, ioDecompositionContext.GetCurrentNodePathMutable()),
      mIndices(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetIndicesMutable())
{
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
