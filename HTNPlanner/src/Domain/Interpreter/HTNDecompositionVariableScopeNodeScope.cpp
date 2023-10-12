#include "Domain/Interpreter/HTNDecompositionVariableScopeNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNVariables.h"

HTNDecompositionVariableScopeNodeScope::HTNDecompositionVariableScopeNodeScope(HTNDecompositionContext& outDecompositionContext,
                                                                               const std::string&       inNodeID)
    : HTNNodeScope(outDecompositionContext.GetCurrentVariableScopeNodePathMutable(), inNodeID),
      mVariables(outDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetVariablesMutable())
{
}

HTNDecompositionVariableScopeNodeScope::~HTNDecompositionVariableScopeNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the variables associated to the node path
    const std::string& NodePath = mNodePath.GetNodePath();
    mVariables.RemoveVariables(NodePath);
}
