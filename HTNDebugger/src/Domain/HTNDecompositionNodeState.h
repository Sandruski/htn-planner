// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Domain/HTNDecompositionHelpers.h"

#include <map>

enum class HTNNodeStep : uint8;

/**
 * Base decomposition node state
 */
class HTNDecompositionNodeStateBase
{
public:
    HTNDecompositionNodeStateBase() = default;
    explicit HTNDecompositionNodeStateBase(const int32 inDecompositionStep);
    virtual ~HTNDecompositionNodeStateBase() = 0;

    void SetDecompositionStep(const int32 inDecompositionStep);
    HTN_NODISCARD int32 GetDecompositionStep() const;

    HTN_NODISCARD virtual HTNNodeStep GetNodeStep() const = 0;

private:
    // Decomposition step of the node
    int32 mDecompositionStep = HTNDecompositionHelpers::kInvalidDecompositionStep;
};

/**
 * Decomposition node state (not choice point)
 * - A decomposition step of -1 means use current decomposition step
 */
class HTNDecompositionNodeState final : public HTNDecompositionNodeStateBase
{
public:
    HTNDecompositionNodeState() = default;
    explicit HTNDecompositionNodeState(const int32 inDecompositionStep, const bool inIsOpen);

    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    void SetIsOpen(const bool inIsOpen);
    HTN_NODISCARD bool IsOpen() const;

private:
    // Whether the node is open or closed
    bool mIsOpen = false;
};

/**
 * Decomposition choice point node state
 * - A decomposition step of -1 means use all decomposition steps
 */
class HTNDecompositionChoicePointNodeState final : public HTNDecompositionNodeStateBase
{
public:
    HTNDecompositionChoicePointNodeState() = default;
    explicit HTNDecompositionChoicePointNodeState(const int32 inDecompositionStep, const bool inIsOpen);

    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    void SetIsOpen(const size inDecompositionStep, const bool inIsOpen);
    HTN_NODISCARD bool IsOpen(const size inDecompositionStep) const;
    HTN_NODISCARD int32 FindOpenDecompositionStepInRange(const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep) const;

private:
    // Decomposition step to whether the node is open or closed
    std::map<size, bool> mIsOpen;
};

inline void HTNDecompositionNodeStateBase::SetDecompositionStep(const int32 inDecompositionStep)
{
    mDecompositionStep = inDecompositionStep;
}

inline int32 HTNDecompositionNodeStateBase::GetDecompositionStep() const
{
    return mDecompositionStep;
}

inline void HTNDecompositionNodeState::SetIsOpen(const bool inIsOpen)
{
    mIsOpen = inIsOpen;
}

inline bool HTNDecompositionNodeState::IsOpen() const
{
    return mIsOpen;
}

inline void HTNDecompositionChoicePointNodeState::SetIsOpen(const size inDecompositionStep, const bool inIsOpen)
{
    mIsOpen[inDecompositionStep] = inIsOpen;
}

inline bool HTNDecompositionChoicePointNodeState::IsOpen(const size inDecompositionStep) const
{
    const auto It = mIsOpen.find(inDecompositionStep);
    return (It != mIsOpen.end()) ? It->second : false;
}
#endif
