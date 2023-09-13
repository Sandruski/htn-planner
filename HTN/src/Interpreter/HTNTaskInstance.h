#pragma once

#include <memory>
#include <string>

class HTNTaskNode;
class HTNNodeBase;

class HTNTaskInstance
{
public:
    explicit HTNTaskInstance(const std::shared_ptr<const HTNTaskNode>& inTaskNode, const std::string& inScopeID);
    ~HTNTaskInstance();

    const std::shared_ptr<const HTNTaskNode>& GetTaskNode() const;
    const std::string&                        GetScopeID() const;

private:
    std::shared_ptr<const HTNTaskNode> mTaskNode;
    std::string                        mScopeID;
};

inline const std::shared_ptr<const HTNTaskNode>& HTNTaskInstance::GetTaskNode() const
{
    return mTaskNode;
}

inline const std::string& HTNTaskInstance::GetScopeID() const
{
    return mScopeID;
}
