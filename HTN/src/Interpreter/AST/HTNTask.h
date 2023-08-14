#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNBranch;
class HTNValue;

enum class HTNTaskType
{
    PRIMITIVE,
    COMPOUND,
};

class HTNTask : public HTNNodeBase
{
public:
    explicit HTNTask(const HTNTaskType inType, std::unique_ptr<const HTNValue> inName,
                     const std::vector<std::shared_ptr<const HTNValue>>& inArguments);
    ~HTNTask();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    HTNTaskType                                         GetType() const;
    std::string                                         GetName() const;
    const std::vector<std::shared_ptr<const HTNValue>>& GetArguments() const;
    void                                                SetParent(const std::weak_ptr<const HTNBranch>& inParent);
    const std::weak_ptr<const HTNBranch>&               GetParent() const;

private:
    HTNTaskType                                  mType;
    std::unique_ptr<const HTNValue>              mName;
    std::vector<std::shared_ptr<const HTNValue>> mArguments;
    std::weak_ptr<const HTNBranch>               mParent;
};

inline HTNTaskType HTNTask::GetType() const
{
    return mType;
}

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNTask::GetArguments() const
{
    return mArguments;
}

inline void HTNTask::SetParent(const std::weak_ptr<const HTNBranch>& inParent)
{
    mParent = inParent;
}

inline const std::weak_ptr<const HTNBranch>& HTNTask::GetParent() const
{
    return mParent;
}
