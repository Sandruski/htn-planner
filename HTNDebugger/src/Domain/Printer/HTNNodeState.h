// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionHelpers.h"
#include "HTNCoreMinimal.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNChoicePointNodeState;
class HTNNodeState;

enum class HTNNodeStep : uint8;

// Node path to node state
using HTNNodeStates = std::unordered_map<std::string, HTNNodeState>;

// Node path to choice point node state
using HTNChoicePointNodeStates = std::unordered_map<std::string, HTNChoicePointNodeState>;

/**
 * Base class for the state of a node in a decomposition printer
 */
class HTNNodeStateBase
{
public:
    HTNNodeStateBase() = default;
    explicit HTNNodeStateBase(const int32 inDecompositionStep);
    virtual ~HTNNodeStateBase() = 0;

    void                SetDecompositionStep(const int32 inDecompositionStep);
    HTN_NODISCARD int32 GetDecompositionStep() const;

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

    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    void               SetIsOpen(const bool inIsOpen);
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

    HTN_NODISCARD HTNNodeStep GetNodeStep() const final;

    void                SetIsOpen(const size inDecompositionStep, const bool inIsOpen);
    HTN_NODISCARD bool  IsOpen(const size inDecompositionStep) const;
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
