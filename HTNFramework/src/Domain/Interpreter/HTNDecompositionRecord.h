// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNTaskResult.h"

#include <vector>

class HTNDecompositionRecord
{
public:
    void            PushPendingTaskInstance(const HTNTaskInstance& inPendingTaskInstance);
    HTN_NODISCARD HTNTaskInstance PopPendingTaskInstance();
    HTN_NODISCARD bool            HasPendingTaskInstances() const;

    void                  SetEnvironment(const HTNEnvironment& inEnvironment);
    HTN_NODISCARD const HTNEnvironment& GetEnvironment() const;
    HTN_NODISCARD HTNEnvironment&       GetEnvironmentMutable();

    void                              AddTaskResultToPlan(const HTNTaskResult& inTaskResult);
    HTN_NODISCARD const HTNPlan& GetPlan() const;

private:
    // State of variables and indices
    HTNEnvironment mEnvironment;

    // Tasks pending to be processed
    std::vector<HTNTaskInstance> mPendingTaskInstances;

    // Final plan of tasks
    HTNPlan mPlan;
};

inline void HTNDecompositionRecord::PushPendingTaskInstance(const HTNTaskInstance& inPendingTaskInstance)
{
    mPendingTaskInstances.emplace_back(inPendingTaskInstance);
}

inline HTNTaskInstance HTNDecompositionRecord::PopPendingTaskInstance()
{
    const HTNTaskInstance PendingTaskInstance = mPendingTaskInstances.back();
    mPendingTaskInstances.pop_back();

    return PendingTaskInstance;
}

inline bool HTNDecompositionRecord::HasPendingTaskInstances() const
{
    return !mPendingTaskInstances.empty();
}

inline void HTNDecompositionRecord::SetEnvironment(const HTNEnvironment& inEnvironment)
{
    mEnvironment = inEnvironment;
}

inline const HTNEnvironment& HTNDecompositionRecord::GetEnvironment() const
{
    return mEnvironment;
}

inline HTNEnvironment& HTNDecompositionRecord::GetEnvironmentMutable()
{
    return mEnvironment;
}

inline void HTNDecompositionRecord::AddTaskResultToPlan(const HTNTaskResult& inTaskResult)
{
    mPlan.emplace_back(inTaskResult);
}

inline const HTNPlan& HTNDecompositionRecord::GetPlan() const
{
    return mPlan;
}
