#include "Interpreter/HTNDecompositionScope.h"

#include "Interpreter/HTNDecompositionContext.h"

HTNDecompositionScope::HTNDecompositionScope(HTNDecompositionContext& ioDecompositionContext) : mDecompositionContext(ioDecompositionContext)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PushEnvironment();
}

HTNDecompositionScope::~HTNDecompositionScope()
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PopEnvironment();
}
