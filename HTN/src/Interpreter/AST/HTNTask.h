#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

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

private:
    HTNTaskType                                  mType;
    std::unique_ptr<const HTNValue>              mName;
    std::vector<std::shared_ptr<const HTNValue>> mArguments;
};

inline HTNTaskType HTNTask::GetType() const
{
    return mType;
}

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNTask::GetArguments() const
{
    return mArguments;
}
