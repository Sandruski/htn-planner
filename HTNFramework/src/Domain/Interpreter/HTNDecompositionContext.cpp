// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNDecompositionContext.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeSnapshot.h"
#endif

HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState, const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                 const std::string& inEntryPointID)
    : mWorldState(inWorldState), mDomainNode(inDomainNode), mEntryPointID(inEntryPointID)
{
}

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionRecordHistory.empty())
    {
        return false;
    }

    mCurrentDecomposition = mDecompositionRecordHistory.back();
    mDecompositionRecordHistory.pop_back();

#ifdef HTN_DEBUG_DECOMPOSITION
    mDecompositionSnapshot.IncrementCurrentDecompositionStep();
#endif

    return true;
}

#ifdef HTN_DEBUG_DECOMPOSITION
void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment      = mCurrentDecomposition.GetEnvironment();
    const HTNVariablesManager& VariablesManager = Environment.GetVariablesManager();
    const HTNNodeSnapshot      NodeSnapshot     = HTNNodeSnapshot(VariablesManager);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}

void HTNDecompositionContext::RecordNodeSnapshot(const std::string& inNodePath, const bool inNodeResult, const HTNNodeStep inNodeStep,
                                                 const bool inIsChoicePoint)
{
    const HTNEnvironment&      Environment      = mCurrentDecomposition.GetEnvironment();
    const HTNVariablesManager& VariablesManager = Environment.GetVariablesManager();
    const HTNNodeSnapshot      NodeSnapshot     = HTNNodeSnapshot(VariablesManager, inNodeResult);
    mDecompositionSnapshot.AddNodeSnapshot(inNodePath, NodeSnapshot, inNodeStep, inIsChoicePoint);
}
#endif
