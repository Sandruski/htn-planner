#include "Interpreter/AST/HTNTask.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNTask::HTNTask(const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const HTNTaskType inType) : mArguments(inArguments), mType(inType)
{
}

HTNTask::~HTNTask() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNTask::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNTask::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNTask::ToString() const
{
    std::string Description;

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Description.append(std::format("{} ", Argument->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}
