// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNNodeState.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeStep.h"

HTNNodeStateBase::HTNNodeStateBase(const int32 inDecompositionStep) : mDecompositionStep(inDecompositionStep)
{
}

HTNNodeState::HTNNodeState(const int32 inDecompositionStep, const bool inIsOpen)
    : HTNNodeStateBase(inDecompositionStep), mIsOpen(inIsOpen)
{
}

HTNNodeStateBase::~HTNNodeStateBase() = default;

HTNNodeStep HTNNodeState::GetNodeStep() const
{
    const int32 DecompositionStep = GetDecompositionStep();
    return HTNDecompositionHelpers::IsDecompositionStepValid(DecompositionStep) ? HTNNodeStep::END : HTNNodeStep::START;
}

HTNChoicePointNodeState::HTNChoicePointNodeState(const int32 inDecompositionStep, const bool inIsOpen)
    : HTNNodeStateBase(inDecompositionStep), mIsOpen({{inDecompositionStep, inIsOpen}})
{
}

HTNNodeStep HTNChoicePointNodeState::GetNodeStep() const
{
    return HTNNodeStep::END;
}

int32 HTNChoicePointNodeState::FindOpenDecompositionStepInRange(const int32 inMinDecompositionStep,
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
