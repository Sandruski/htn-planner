// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNVariables.h"
#include "HTNCoreMinimal.h"

class HTNNodeResult
{
public:
    HTNNodeResult() = default;
    explicit HTNNodeResult(const HTNVariables& inVariables);
    explicit HTNNodeResult(const HTNVariables& inVariables, const bool inResult);

    HTN_NODISCARD const HTNVariables& GetVariables() const;
    HTN_NODISCARD bool                 GetResult() const;

private:
    HTNVariables mVariables;
    bool         mResult = false;
};

inline bool HTNNodeResult::GetResult() const
{
    return mResult;
}

inline const HTNVariables& HTNNodeResult::GetVariables() const
{
    return mVariables;
}
#endif
