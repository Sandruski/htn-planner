#include "Domain/Interpreter/HTNDecompositionContext.h"

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionHistory.empty())
    {
        return false;
    }

    mCurrentDecomposition = mDecompositionHistory.back();
    mDecompositionHistory.pop_back();

#ifdef HTN_DEBUG
    mDecompositionSnapshot.IncrementDecompositionStep();
#endif

    return true;
}

#ifdef HTN_DEBUG
void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const bool inResult, const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(inResult, Variables);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inIsChoicePoint);
}
#endif
