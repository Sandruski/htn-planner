#include "Domain/HTNDecompositionNodeState.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

HTNDecompositionNodeStateBase::HTNDecompositionNodeStateBase(const int inDecompositionStep) : mDecompositionStep(inDecompositionStep)
{
}

HTNDecompositionNodeState::HTNDecompositionNodeState(const int inDecompositionStep, const bool inIsOpen)
    : HTNDecompositionNodeStateBase(inDecompositionStep), mIsOpen(inIsOpen)
{
}

HTNNodeStep HTNDecompositionNodeState::GetNodeStep() const
{
    const int DecompositionStep = GetDecompositionStep();
    return HTNDecompositionHelpers::IsDecompositionStepValid(DecompositionStep) ? HTNNodeStep::END : HTNNodeStep::START;
}

HTNDecompositionChoicePointNodeState::HTNDecompositionChoicePointNodeState(const int inDecompositionStep, const bool inIsOpen)
    : HTNDecompositionNodeStateBase(inDecompositionStep), mIsOpen({{inDecompositionStep, inIsOpen}})
{
}

HTNNodeStep HTNDecompositionChoicePointNodeState::GetNodeStep() const
{
    return HTNNodeStep::END;
}

int HTNDecompositionChoicePointNodeState::FindOpenDecompositionStepInRange(const int inMinDecompositionStep, const int inMaxDecompositionStep) const
{
    for (const std::pair<std::size_t, bool>& Pair : mIsOpen)
    {
        const int DecompositionStep = static_cast<int>(Pair.first);
        if (!HTNDecompositionHelpers::IsDecompositionStepInRange(DecompositionStep, inMinDecompositionStep, inMaxDecompositionStep))
        {
            continue;
        }

        const bool IsOpen = Pair.second;
        if (!IsOpen)
        {
            continue;
        }

        return DecompositionStep;
    }

    return HTNDecompositionHelpers::kInvalidDecompositionStep;
}
#endif
