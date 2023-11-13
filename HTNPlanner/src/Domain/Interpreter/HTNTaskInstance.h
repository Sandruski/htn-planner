#pragma once

#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNNodePath.h"

#include <memory>

class HTNTaskNodeBase;

class HTNTaskInstance
{
public:
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                             const HTNNodePath& inNodePath, const HTNNodePath& inVariableScopeNodePath);

    const std::shared_ptr<const HTNTaskNodeBase>& GetTaskNode() const;
    const HTNEnvironment&                         GetEnvironment() const;
    const HTNNodePath&                            GetNodePath() const;
    const HTNNodePath&                            GetVariableScopeNodePath() const;

private:
    std::shared_ptr<const HTNTaskNodeBase> mTaskNode;
    HTNEnvironment                         mEnvironment;
    HTNNodePath                            mNodePath;
    HTNNodePath                            mVariableScopeNodePath;
};

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const HTNEnvironment& HTNTaskInstance::GetEnvironment() const
{
    return mEnvironment;
}

inline const HTNNodePath& HTNTaskInstance::GetNodePath() const
{
    return mNodePath;
}

inline const HTNNodePath& HTNTaskInstance::GetVariableScopeNodePath() const
{
    return mVariableScopeNodePath;
}
