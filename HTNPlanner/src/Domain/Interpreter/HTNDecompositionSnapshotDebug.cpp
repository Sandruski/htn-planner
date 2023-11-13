#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

#ifdef HTN_DEBUG
HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const HTNVariables& inVariables) : mVariables(inVariables)
{
}

HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const HTNVariables& inVariables, const bool inResult) : mVariables(inVariables), mResult(inResult)
{
}

HTNNodeSnapshotHistoryDebug::HTNNodeSnapshotHistoryDebug(const bool inIsChoicePoint) : mIsChoicePoint(inIsChoicePoint)
{
}

void HTNDecompositionSnapshotDebug::AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot,
                                                    const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    if (!mNodeSnapshotHistoryCollection.contains(inNodePath))
    {
        mNodeSnapshotHistoryCollection.emplace(inNodePath, HTNNodeSnapshotHistoryDebug(inIsChoicePoint));
    }

    HTNNodeSnapshotHistoryDebug& NodeSnapshotHistory = mNodeSnapshotHistoryCollection[inNodePath];
    assert(inIsChoicePoint == NodeSnapshotHistory.IsChoicePoint());

    HTNNodeSnapshotStepsCollectionDebug& NodeSnapshotStepsCollection = NodeSnapshotHistory.GetNodeSnapshotStepsCollectionMutable();
    HTNNodeSnapshotCollectionDebug&      NodeSnapshotCollection      = NodeSnapshotStepsCollection[mDecompositionStep];
    NodeSnapshotCollection[inNodeStep]                               = inNodeSnapshot;
}
#endif
