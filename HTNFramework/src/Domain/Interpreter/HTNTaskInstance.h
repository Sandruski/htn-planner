// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNPathManager.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNTaskNodeBase;

/*
 * Primitive or compound task in a decomposition
 */
class HTNTaskInstance
{
public:
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                             const HTNPathManager& inNodePathManager, const HTNPathManager& inVariablesPathManager);

    // Returns the task node
    HTN_NODISCARD const std::shared_ptr<const HTNTaskNodeBase>& GetTaskNode() const;

    // Returns the environment
    HTN_NODISCARD const HTNEnvironment& GetEnvironment() const;

    // Returns the node path manager
    HTN_NODISCARD const HTNPathManager& GetNodePathManager() const;

    // Returns the variables path manager
    HTN_NODISCARD const HTNPathManager& GetVariablesPathManager() const;

private:
    std::shared_ptr<const HTNTaskNodeBase> mTaskNode;
    HTNEnvironment                         mEnvironment;
    HTNPathManager                         mNodePathManager;
    HTNPathManager                         mVariablesPathManager;
};

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const HTNEnvironment& HTNTaskInstance::GetEnvironment() const
{
    return mEnvironment;
}

inline const HTNPathManager& HTNTaskInstance::GetNodePathManager() const
{
    return mNodePathManager;
}

inline const HTNPathManager& HTNTaskInstance::GetVariablesPathManager() const
{
    return mVariablesPathManager;
}
