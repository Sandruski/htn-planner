#include "Domain/Interpreter/HTNVariableScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"

HTNVariableScope::HTNVariableScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID)
    : mDecompositionContext(ioDecompositionContext)
{
    mResult = mDecompositionContext.TryPushNodeToCurrentVariableScopePath(inNodeID);
}

HTNVariableScope::~HTNVariableScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the variables associated to the current scope
    const std::string       CurrentVariableScopePath = mDecompositionContext.GetCurrentVariableScopePath();
    HTNDecompositionRecord& CurrentDecomposition     = mDecompositionContext.GetCurrentDecompositionMutable();
    HTNEnvironment&         Environment              = CurrentDecomposition.GetEnvironmentMutable();
    Environment.RemoveVariables(CurrentVariableScopePath);

    // Remove the current node from the path
    const bool Result = mDecompositionContext.TryPopNodeFromCurrentVariableScopePath();
    assert(Result);
}
