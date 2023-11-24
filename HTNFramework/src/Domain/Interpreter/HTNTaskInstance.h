// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Core/HTNPathHandler.h"

#include <memory>

class HTNTaskNodeBase;

class HTNTaskInstance
{
public:
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                             const HTNPathHandler& inNodePathHandler, const HTNPathHandler& inVariablesPathHandler);

    HTN_NODISCARD const std::shared_ptr<const HTNTaskNodeBase>& GetTaskNode() const;
    HTN_NODISCARD const HTNEnvironment&                         GetEnvironment() const;
    HTN_NODISCARD const HTNPathHandler&                         GetNodePathHandler() const;
    HTN_NODISCARD const HTNPathHandler&                         GetVariablesPathHandler() const;

private:
    std::shared_ptr<const HTNTaskNodeBase> mTaskNode;
    HTNEnvironment                         mEnvironment;
    HTNPathHandler                         mNodePathHandler;
    HTNPathHandler                         mVariablesPathHandler;
};

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const HTNEnvironment& HTNTaskInstance::GetEnvironment() const
{
    return mEnvironment;
}

inline const HTNPathHandler& HTNTaskInstance::GetNodePathHandler() const
{
    return mNodePathHandler;
}

inline const HTNPathHandler& HTNTaskInstance::GetVariablesPathHandler() const
{
    return mVariablesPathHandler;
}
