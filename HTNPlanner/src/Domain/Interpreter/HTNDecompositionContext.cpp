#include "Domain/Interpreter/HTNDecompositionContext.h"

HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState) : mWorldState(inWorldState)
{
}

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
void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const bool inResult, const HTNNodeStep inNodeStep,
                                                 const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(Variables, inResult);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}

void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment  = mCurrentDecomposition.GetEnvironment();
    const HTNVariables&        Variables    = Environment.GetVariables();
    const HTNNodeSnapshotDebug NodeSnapshot = HTNNodeSnapshotDebug(Variables);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}
#endif
