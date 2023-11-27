// Copyright(c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNDecompositionSnapshot.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeSnapshotHistory::HTNNodeSnapshotHistory(const bool inIsChoicePoint) : mIsChoicePoint(inIsChoicePoint)
{
}

void HTNDecompositionSnapshot::AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshot& inNodeSnapshot, const HTNNodeStep inNodeStep,
                                               const bool inIsChoicePoint)
{
    if (!mNodeSnapshotHistoryCollection.contains(inNodePath))
    {
        mNodeSnapshotHistoryCollection.emplace(inNodePath, HTNNodeSnapshotHistory(inIsChoicePoint));
    }

    HTNNodeSnapshotHistory& NodeSnapshotHistory = mNodeSnapshotHistoryCollection[inNodePath];
    assert(inIsChoicePoint == NodeSnapshotHistory.IsChoicePoint());

    HTNNodeSnapshotStepsCollection& NodeSnapshotStepsCollection = NodeSnapshotHistory.GetNodeSnapshotStepsCollectionMutable();
    HTNNodeSnapshotCollection&      NodeSnapshotCollection      = NodeSnapshotStepsCollection[mCurrentDecompositionStep];
    NodeSnapshotCollection[inNodeStep]                          = inNodeSnapshot;
}
#endif
