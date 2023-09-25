#pragma once

#include "Interpreter/HTNEnvironment.h"

#include <memory>

class HTNEnvironment;
class HTNTaskNodeBase;

class HTNTaskInstance
{
public:
    HTNTaskInstance() = default;
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment);
    ~HTNTaskInstance();

    const std::shared_ptr<const HTNTaskNodeBase>& GetTaskNode() const;
    const HTNEnvironment&                         GetEnvironment() const;

private:
    std::shared_ptr<const HTNTaskNodeBase> mTaskNode;
    HTNEnvironment mEnvironment;
};

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const HTNEnvironment& HTNTaskInstance::GetEnvironment() const
{
    return mEnvironment;
}
