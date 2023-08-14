#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNDecompositionContext;
class HTNTask;
class HTNValue;
class HTNMethod;

class HTNBranch final : public HTNNodeBase
{
public:
    explicit HTNBranch(std::unique_ptr<const HTNValue> inName);
    ~HTNBranch();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                                        GetName() const;
    void                                               SetPreCondition(const std::shared_ptr<const HTNConditionBase>& inPreCondition);
    const std::shared_ptr<const HTNConditionBase>&     GetPreCondition() const;
    void                                               SetTasks(const std::vector<std::shared_ptr<const HTNTask>>& inTasks);
    const std::vector<std::shared_ptr<const HTNTask>>& GetTasks() const;
    void                                               SetParent(const std::weak_ptr<const HTNMethod>& inParent);
    const std::weak_ptr<const HTNMethod>&              GetParent() const;

private:
    std::unique_ptr<const HTNValue>             mName;
    std::shared_ptr<const HTNConditionBase>     mPreCondition;
    std::vector<std::shared_ptr<const HTNTask>> mTasks;
    std::weak_ptr<const HTNMethod>              mParent;
};

inline void HTNBranch::SetPreCondition(const std::shared_ptr<const HTNConditionBase>& inPreCondition)
{
    mPreCondition = inPreCondition;
}

inline const std::shared_ptr<const HTNConditionBase>& HTNBranch::GetPreCondition() const
{
    return mPreCondition;
}

inline void HTNBranch::SetTasks(const std::vector<std::shared_ptr<const HTNTask>>& inTasks)
{
    mTasks = inTasks;
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNBranch::GetTasks() const
{
    return mTasks;
}

inline void HTNBranch::SetParent(const std::weak_ptr<const HTNMethod>& inParent)
{
    mParent = inParent;
}

inline const std::weak_ptr<const HTNMethod>& HTNBranch::GetParent() const
{
    return mParent;
}
