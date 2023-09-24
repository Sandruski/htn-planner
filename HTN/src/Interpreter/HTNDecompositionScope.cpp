#include "Interpreter/HTNDecompositionScope.h"

#include "Interpreter/HTNDecompositionContext.h"

HTNDecompositionScope::HTNDecompositionScope(HTNDecompositionContext& ioDecompositionContext)
    : mDecompositionContext(ioDecompositionContext)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PushEnvironment();
}

HTNDecompositionScope::HTNDecompositionScope(HTNDecompositionContext& ioDecompositionContext, const HTNEnvironment& inEnvironment)
    : mDecompositionContext(ioDecompositionContext)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PushEnvironment(inEnvironment);
}

HTNDecompositionScope::~HTNDecompositionScope()
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PopEnvironment();
}
