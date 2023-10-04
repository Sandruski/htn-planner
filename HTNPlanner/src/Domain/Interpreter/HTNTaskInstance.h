#pragma once

#include "Domain/Interpreter/HTNEnvironment.h"

#include <memory>

class HTNEnvironment;
class HTNTaskNodeBase;

class HTNTaskInstance
{
public:
    HTNTaskInstance() = default;
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                             const std::string& inVariableScopePath);
    ~HTNTaskInstance();

    const std::shared_ptr<const HTNTaskNodeBase>& GetTaskNode() const;
    const HTNEnvironment&                         GetEnvironment() const;
    const std::string&                            GetVariableScopePath() const;

private:
    std::shared_ptr<const HTNTaskNodeBase> mTaskNode;
    HTNEnvironment                         mEnvironment;
    std::string                            mVariableScopePath;
};

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const HTNEnvironment& HTNTaskInstance::GetEnvironment() const
{
    return mEnvironment;
}

inline const std::string& HTNTaskInstance::GetVariableScopePath() const
{
    return mVariableScopePath;
}