// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "HTNCoreMinimal.h"

/**
 * Result of a node in a decomposition
 * - TODO salvarez Make this a base class with two derived classes - one with the result and one without it
 */
class HTNNodeResult
{
public:
    HTNNodeResult() = default;
    explicit HTNNodeResult(const HTNVariablesManager& inVariablesManager);
    explicit HTNNodeResult(const HTNVariablesManager& inVariablesManager, const bool inResult);

    // Returns the variables manager of the node
    HTN_NODISCARD const HTNVariablesManager& GetVariablesManager() const;

    // Returns the actual result of the node
    HTN_NODISCARD bool GetResult() const;

private:
    // Variables manager of the node
    HTNVariablesManager mVariablesManager;

    // Actual result of the node
    bool mResult = false;
};

inline bool HTNNodeResult::GetResult() const
{
    return mResult;
}

inline const HTNVariablesManager& HTNNodeResult::GetVariablesManager() const
{
    return mVariablesManager;
}
#endif
