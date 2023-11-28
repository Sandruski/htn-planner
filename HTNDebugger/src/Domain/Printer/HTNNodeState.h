// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionHelpers.h"
#include "Domain/Printer/HTNNodeStateFwd.h"
#include "HTNCoreMinimal.h"

#include <map>

enum class HTNNodeStep : uint8;

/**
 * Base class for the state of a node in a decomposition printer
 */
class HTNNodeStateBase
{
public:
    HTNNodeStateBase() = default;
    explicit HTNNodeStateBase(const int32 inDecompositionStep);
    virtual ~HTNNodeStateBase() = 0;

    // Sets the decomposition step to the given one
    void SetDecompositionStep(const int32 inDecompositionStep);

    // Returns the decomposition step
    HTN_NODISCARD int32 GetDecompositionStep() const;

    // Returns the node step
    HTN_NODISCARD virtual HTNNodeStep GetNodeStep() const = 0;

private:
    // Decomposition step of the node
    int32 mDecompositionStep = HTNDecompositionHelpers::kInvalidDecompositionStep;
};

/**
 * State of a node that is not a choice point in a decomposition printer
 * - A decomposition step of -1 means print the current decomposition step
 */
class HTNNodeState final : public HTNNodeStateBase
{
public:
    HTNNodeState() = default;
    explicit HTNNodeState(const int32 inDecompositionStep, const bool inIsOpen);

    // Returns the node step
    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    // Sets whether the node is open or closed
    void SetIsOpen(const bool inIsOpen);

    // Returns whether the node is open or closed
    HTN_NODISCARD bool IsOpen() const;

private:
    // Whether the node is open or closed
    bool mIsOpen = false;
};

/**
 * State of a choice point node in a decomposition printer
 * - A decomposition step of -1 means print all decomposition steps
 */
class HTNChoicePointNodeState final : public HTNNodeStateBase
{
public:
    HTNChoicePointNodeState() = default;
    explicit HTNChoicePointNodeState(const int32 inDecompositionStep, const bool inIsOpen);

    // Returns the node step
    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    // Sets whether the node is open or closed at the given decomposition step
    void SetIsOpen(const size inDecompositionStep, const bool inIsOpen);

    // Returns whether the node is open or closed at the given decomposition step
    HTN_NODISCARD bool IsOpen(const size inDecompositionStep) const;

    // Returns a decomposition step in which the node is open in the given range
    HTN_NODISCARD int32 FindOpenDecompositionStepInRange(const int32 inMinDecompositionStep, const int32 inMaxDecompositionStep) const;

private:
    // Decomposition step of the node to whether the node is open or closed
    std::map<size, bool> mIsOpen;
};

inline void HTNNodeStateBase::SetDecompositionStep(const int32 inDecompositionStep)
{
    mDecompositionStep = inDecompositionStep;
}

inline int32 HTNNodeStateBase::GetDecompositionStep() const
{
    return mDecompositionStep;
}

inline void HTNNodeState::SetIsOpen(const bool inIsOpen)
{
    mIsOpen = inIsOpen;
}

inline bool HTNNodeState::IsOpen() const
{
    return mIsOpen;
}

inline void HTNChoicePointNodeState::SetIsOpen(const size inDecompositionStep, const bool inIsOpen)
{
    mIsOpen[inDecompositionStep] = inIsOpen;
}

inline bool HTNChoicePointNodeState::IsOpen(const size inDecompositionStep) const
{
    const auto It = mIsOpen.find(inDecompositionStep);
    return (It != mIsOpen.cend()) ? It->second : false;
}
#endif
