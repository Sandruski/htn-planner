// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "HTNCoreMinimal.h"

#include <vector>

/**
 * Record of a decomposition
 */
class HTNDecompositionRecord
{
public:
    // Pushes the given task instance to the pending ones
    void PushPendingTaskInstance(const HTNTaskInstance& inTaskInstance);

    // Pops a task instance from the pending ones
    HTN_NODISCARD HTNTaskInstance PopPendingTaskInstance();

    // Returns whether there are pending task instances
    HTN_NODISCARD bool HasPendingTaskInstances() const;

    // Sets the environment to the given one
    void SetEnvironment(const HTNEnvironment& inEnvironment);

    // Returns the environment
    HTN_NODISCARD const HTNEnvironment& GetEnvironment() const;

    // Returns the environment
    HTN_NODISCARD HTNEnvironment& GetEnvironmentMutable();

    // Adds the given task result to the plan
    void AddTaskResultToPlan(const HTNTaskResult& inTaskResult);

    // Returns the plan
    HTN_NODISCARD const HTNPlan& GetPlan() const;

private:
    HTNEnvironment mEnvironment;

    // Stack of tasks pending to be processed
    std::vector<HTNTaskInstance> mPendingTaskInstances;

    HTNPlan mPlan;
};

inline void HTNDecompositionRecord::PushPendingTaskInstance(const HTNTaskInstance& inTaskInstance)
{
    mPendingTaskInstances.emplace_back(inTaskInstance);
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
