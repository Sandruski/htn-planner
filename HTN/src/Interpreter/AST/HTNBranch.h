#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNTask;
class HTNValue;

class HTNBranch final : public HTNNodeBase
{
public:
    explicit HTNBranch(const std::shared_ptr<const HTNValue>& inIDArgument, const std::shared_ptr<const HTNConditionBase>& inPreCondition,
                       const std::vector<std::shared_ptr<const HTNTask>>& inTasks);
    ~HTNBranch();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::shared_ptr<const HTNValue>&             GetIDArgument() const;
    const std::shared_ptr<const HTNConditionBase>&     GetPreCondition() const;
    const std::vector<std::shared_ptr<const HTNTask>>& GetTasks() const;

private:
    // ID of the branch, which is unique within its method
    std::shared_ptr<const HTNValue> mIDArgument;

    // Pre-condition of the branch
    std::shared_ptr<const HTNConditionBase> mPreCondition;

    // Tasks of the branch
    std::vector<std::shared_ptr<const HTNTask>> mTasks;
};

inline const std::shared_ptr<const HTNValue>& HTNBranch::GetIDArgument() const
{
    return mIDArgument;
}

inline const std::shared_ptr<const HTNConditionBase>& HTNBranch::GetPreCondition() const
{
    return mPreCondition;
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNBranch::GetTasks() const
{
    return mTasks;
}
