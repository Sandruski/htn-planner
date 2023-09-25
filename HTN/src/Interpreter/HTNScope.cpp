#include "Interpreter/HTNScope.h"

#include "Interpreter/HTNDecompositionContext.h"

HTNScope::HTNScope(HTNDecompositionContext& ioDecompositionContext)
    : mDecompositionContext(ioDecompositionContext)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PushEnvironment();
}

HTNScope::HTNScope(HTNDecompositionContext& ioDecompositionContext, const HTNEnvironment& inEnvironment)
    : mDecompositionContext(ioDecompositionContext)
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PushEnvironment(inEnvironment);
}

HTNScope::~HTNScope()
{
    HTNDecompositionRecord& CurrentDecomposition = mDecompositionContext.GetCurrentDecompositionMutable();
    CurrentDecomposition.PopEnvironment();
}
