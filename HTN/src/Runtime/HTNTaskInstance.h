#pragma once

#include <memory>
#include <string>

class HTNTask;
class HTNNodeBase;

class HTNTaskInstance
{
public:
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTask>& inTask, const std::string& inScopeID);
    ~HTNTaskInstance();

    const std::shared_ptr<const HTNTask>& GetTask() const;
    const std::string&                    GetScopeID() const;

private:
    std::shared_ptr<const HTNTask> mTask;
    std::string                    mScopeID;
};

inline const std::shared_ptr<const HTNTask>& HTNTaskInstance::GetTask() const
{
    return mTask;
}

inline const std::string& HTNTaskInstance::GetScopeID() const
{
    return mScopeID;
}
