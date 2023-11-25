// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Core/HTNAtom.h"

#include <string>
#include <vector>

class HTNTaskResult;

using HTNPlan = std::vector<HTNTaskResult>;

/*
 * Result of a primitive task
 */
class HTNTaskResult
{
public:
    explicit HTNTaskResult(const std::string& inID, const std::vector<HTNAtom>& inArguments);

    HTN_NODISCARD const std::string&          GetID() const;
    HTN_NODISCARD const std::vector<HTNAtom>& GetArguments() const;

private:
    // ID of the task
    std::string mID;

    // Arguments of the task
    std::vector<HTNAtom> mArguments;
};

inline const std::string& HTNTaskResult::GetID() const
{
    return mID;
}

inline const std::vector<HTNAtom>& HTNTaskResult::GetArguments() const
{
    return mArguments;
}
