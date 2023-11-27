// Copyright(c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNDecompositionResult.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeResultHistory::HTNNodeResultHistory(const bool inIsChoicePoint) : mIsChoicePoint(inIsChoicePoint)
{
}

void HTNDecompositionResult::AddNodeResult(const std::string& inNodePath, const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep,
                                           const bool inIsChoicePoint)
{
    if (!mNodeResultHistoryCollection.contains(inNodePath))
    {
        mNodeResultHistoryCollection.emplace(inNodePath, HTNNodeResultHistory(inIsChoicePoint));
    }

    HTNNodeResultHistory& NodeResultHistory = mNodeResultHistoryCollection[inNodePath];
    assert(inIsChoicePoint == NodeResultHistory.IsChoicePoint());

    HTNNodeResultStepsCollection& NodeResultStepsCollection = NodeResultHistory.GetNodeResultStepsCollectionMutable();
    HTNNodeResultCollection&      NodeResultCollection      = NodeResultStepsCollection[mCurrentDecompositionStep];
    NodeResultCollection[inNodeStep]                        = inNodeResult;
}
#endif
