#include "Interpreter/AST/HTNTask.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNTask::HTNTask(const HTNTaskType inType, std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
    : mType(inType), mName(std::move(inName)), mArguments(inArguments)
{
}

HTNTask::~HTNTask() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNTask::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNTask::ToString() const
{
    std::string Name = GetName();

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Name.append(std::format(" {}", Argument->ToString()));
    }

    return Name;
}

std::string HTNTask::GetName() const
{
    return mName->ToString();
}
