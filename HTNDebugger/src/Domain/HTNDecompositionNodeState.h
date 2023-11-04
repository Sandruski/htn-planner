#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionHelpers.h"

#include <map>

enum class HTNNodeStep : unsigned char;

/**
 * Base decomposition node state
 */
class HTNDecompositionNodeStateBase
{
public:
    HTNDecompositionNodeStateBase() = default;
    explicit HTNDecompositionNodeStateBase(const int inDecompositionStep);
    virtual ~HTNDecompositionNodeStateBase() = default;

    void SetDecompositionStep(const int inDecompositionStep);
    int  GetDecompositionStep() const;

    virtual HTNNodeStep GetNodeStep() const = 0;

private:
    // Decomposition step of the node
    int mDecompositionStep = HTNDecompositionHelpers::kInvalidDecompositionStep;
};

/**
 * Decomposition node state (not choice point)
 * - A decomposition step of -1 means use current decomposition step
 */
class HTNDecompositionNodeState final : public HTNDecompositionNodeStateBase
{
public:
    HTNDecompositionNodeState() = default;
    explicit HTNDecompositionNodeState(const int inDecompositionStep, const bool inIsOpen);

    HTNNodeStep GetNodeStep() const final;

    void SetIsOpen(const bool inIsOpen);
    bool IsOpen() const;

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
    explicit HTNDecompositionChoicePointNodeState(const int inDecompositionStep, const bool inIsOpen);

    HTNNodeStep GetNodeStep() const final;

    void SetIsOpen(const std::size_t inDecompositionStep, const bool inIsOpen);
    bool IsOpen(const std::size_t inDecompositionStep) const;
    int  FindOpenDecompositionStepInRange(const int inMinDecompositionStep, const int inMaxDecompositionStep) const;

private:
    // Decomposition step to whether the node is open or closed
    std::map<std::size_t, bool> mIsOpen;
};

inline void HTNDecompositionNodeStateBase::SetDecompositionStep(const int inDecompositionStep)
{
    mDecompositionStep = inDecompositionStep;
}

inline int HTNDecompositionNodeStateBase::GetDecompositionStep() const
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

inline void HTNDecompositionChoicePointNodeState::SetIsOpen(const std::size_t inDecompositionStep, const bool inIsOpen)
{
    mIsOpen[inDecompositionStep] = inIsOpen;
}

inline bool HTNDecompositionChoicePointNodeState::IsOpen(const std::size_t inDecompositionStep) const
{
    const auto It = mIsOpen.find(inDecompositionStep);
    return ((It != mIsOpen.end()) ? It->second : false);
}
#endif
