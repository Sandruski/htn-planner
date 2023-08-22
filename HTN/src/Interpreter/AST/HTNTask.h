#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNValue;

enum class HTNTaskType
{
    PRIMITIVE,
    COMPOUND,

    INVALID
};

class HTNTask : public HTNNodeBase
{
public:
    explicit HTNTask(const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const HTNTaskType inType);
    ~HTNTask();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::vector<std::shared_ptr<const HTNValue>>& GetArguments() const;
    const std::shared_ptr<const HTNValue>&              GetIDArgument() const;
    HTNTaskType                                         GetType() const;

private:
    // Arguments of the task
    // - 1: ID of the task
    std::vector<std::shared_ptr<const HTNValue>> mArguments;

    // Type of the task
    // - Primitive or compound
    HTNTaskType mType = HTNTaskType::INVALID;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNTask::GetArguments() const
{
    return mArguments;
}

inline const std::shared_ptr<const HTNValue>& HTNTask::GetIDArgument() const
{
    return mArguments[0];
}

inline HTNTaskType HTNTask::GetType() const
{
    return mType;
}
