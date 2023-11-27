// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "HTNCoreMinimal.h"

/**
 * Snapshot of a node in a decomposition
 * - TODO salvarez Make this a base class with two derived classes - one with the result and one without it
 */
class HTNNodeSnapshot
{
public:
    HTNNodeSnapshot() = default;
    explicit HTNNodeSnapshot(const HTNVariablesManager& inVariablesManager);
    explicit HTNNodeSnapshot(const HTNVariablesManager& inVariablesManager, const bool inNodeResult);

    // Returns the variables manager of the node
    HTN_NODISCARD const HTNVariablesManager& GetVariablesManager() const;

    // Returns the result of the node
    HTN_NODISCARD bool GetNodeResult() const;

private:
    // Variables manager of the node
    HTNVariablesManager mVariablesManager;

    // Result of the node
    bool mNodeResult = false;
};

inline bool HTNNodeSnapshot::GetNodeResult() const
{
    return mNodeResult;
}

inline const HTNVariablesManager& HTNNodeSnapshot::GetVariablesManager() const
{
    return mVariablesManager;
}
#endif
