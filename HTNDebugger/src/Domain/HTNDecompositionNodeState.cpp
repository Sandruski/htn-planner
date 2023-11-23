// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionNodeState.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

HTNDecompositionNodeStateBase::HTNDecompositionNodeStateBase(const int32 inDecompositionStep) : mDecompositionStep(inDecompositionStep)
{
}

HTNDecompositionNodeState::HTNDecompositionNodeState(const int32 inDecompositionStep, const bool inIsOpen)
    : HTNDecompositionNodeStateBase(inDecompositionStep), mIsOpen(inIsOpen)
{
}

HTNDecompositionNodeStateBase::~HTNDecompositionNodeStateBase() = default;

HTNNodeStep HTNDecompositionNodeState::GetNodeStep() const
{
    const int32 DecompositionStep = GetDecompositionStep();
    return HTNDecompositionHelpers::IsDecompositionStepValid(DecompositionStep) ? HTNNodeStep::END : HTNNodeStep::START;
}

HTNDecompositionChoicePointNodeState::HTNDecompositionChoicePointNodeState(const int32 inDecompositionStep, const bool inIsOpen)
    : HTNDecompositionNodeStateBase(inDecompositionStep), mIsOpen({{inDecompositionStep, inIsOpen}})
{
}

HTNNodeStep HTNDecompositionChoicePointNodeState::GetNodeStep() const
{
    return HTNNodeStep::END;
}

int32 HTNDecompositionChoicePointNodeState::FindOpenDecompositionStepInRange(const int32 inMinDecompositionStep,
                                                                             const int32 inMaxDecompositionStep) const
{
    for (const std::pair<const size, bool>& Pair : mIsOpen)
    {
        const int32 DecompositionStep = static_cast<const int32>(Pair.first);
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
