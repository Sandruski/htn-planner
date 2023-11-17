#pragma once

#include "HTNPlannerMinimal.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNTaskResult.h"

#include <vector>

class HTNDecompositionRecord
{
public:
    void            PushPendingTaskInstance(const HTNTaskInstance& inPendingTaskInstance);
    HTNTaskInstance PopPendingTaskInstance();
    bool            HasPendingTaskInstances() const;

    void                  SetEnvironment(const HTNEnvironment& inEnvironment);
    const HTNEnvironment& GetEnvironment() const;
    HTNEnvironment&       GetEnvironmentMutable();

    void                              AddTaskResultToPlan(const HTNTaskResult& inTaskResult);
    const std::vector<HTNTaskResult>& GetPlan() const;

private:
    // State of variables and indices
    HTNEnvironment mEnvironment;

    // Tasks pending to be processed
    std::vector<HTNTaskInstance> mPendingTaskInstances;

    // Final plan of tasks
    std::vector<HTNTaskResult> mPlan;
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

inline const std::vector<HTNTaskResult>& HTNDecompositionRecord::GetPlan() const
{
    return mPlan;
}
