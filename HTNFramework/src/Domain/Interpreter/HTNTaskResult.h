// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "HTNCoreMinimal.h"

#include <string>
#include <vector>

class HTNTaskResult;

// Sequence of primitive tasks
using HTNPlan = std::vector<HTNTaskResult>;

/*
 * Result of a primitive task in a decomposition
 */
class HTNTaskResult
{
public:
    explicit HTNTaskResult(const std::string& inTaskID, const std::vector<HTNAtom>& inTaskArguments);

    // Returns the ID of the task
    HTN_NODISCARD const std::string& GetTaskID() const;

    // Returns the arguments of the task
    HTN_NODISCARD const std::vector<HTNAtom>& GetTaskArguments() const;

private:
    std::string          mTaskID;
    std::vector<HTNAtom> mTaskArguments;
};

inline const std::string& HTNTaskResult::GetTaskID() const
{
    return mTaskID;
}

inline const std::vector<HTNAtom>& HTNTaskResult::GetTaskArguments() const
{
    return mTaskArguments;
}
