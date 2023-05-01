#include "Interpreter/AST/HTNTask.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNTask::HTNTask(const EHTNTaskType inType, std::unique_ptr<const HTNValue> inName)
	: mType(inType), mName(std::move(inName))
{
}

HTNTask::HTNTask(const EHTNTaskType inType, std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
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
	return GetName();
}

std::string HTNTask::GetName() const
{
	return mName ? mName->ToString() : "Invalid Task";
}
