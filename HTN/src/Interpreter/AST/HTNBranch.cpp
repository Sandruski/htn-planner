#include "Interpreter/AST/HTNBranch.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"

HTNBranch::HTNBranch(std::unique_ptr<const HTNValue> inName, const std::shared_ptr<const HTNConditionBase>& inCondition, const std::vector<std::shared_ptr<const HTNTask>>& inTasks)
	: mName(std::move(inName)), mCondition(inCondition), mTasks(inTasks)
{
}

HTNBranch::~HTNBranch() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNBranch::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

std::string HTNBranch::ToString() const
{
	return GetName();
}

std::string HTNBranch::GetName() const
{
	return mName ? mName->ToString() : "Invalid Branch";
}
