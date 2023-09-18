#include "Interpreter/HTNDecompositionContext.h"

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionHistory.empty())
    {
        mCurrentDecomposition = HTNDecompositionRecord();
        return false;
    }

    mCurrentDecomposition = mDecompositionHistory.back();
    mDecompositionHistory.pop_back();

    return true;
}
