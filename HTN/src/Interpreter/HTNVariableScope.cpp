#include "Interpreter/HTNVariableScope.h"

#include "Interpreter/HTNDecompositionContext.h"

HTNVariableScope::HTNVariableScope(HTNDecompositionContext& ioDecompositionContext, const std::string& inNodeID)
    : mDecompositionContext(ioDecompositionContext)
{
    mDecompositionContext.PushNodeToCurrentVariableScopePath(inNodeID);
}

HTNVariableScope::~HTNVariableScope()
{
    mDecompositionContext.PopNodeFromCurrentVariableScopePath();
}
