// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "HTNCoreMinimal.h"

class HTNNodeResult
{
public:
    HTNNodeResult() = default;
    explicit HTNNodeResult(const HTNVariablesManager& inVariablesManager);
    explicit HTNNodeResult(const HTNVariablesManager& inVariablesManager, const bool inResult);

    HTN_NODISCARD const HTNVariablesManager& GetVariablesManager() const;
    HTN_NODISCARD bool                       GetResult() const;

private:
    HTNVariablesManager mVariablesManager;
    bool                mResult = false;
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
