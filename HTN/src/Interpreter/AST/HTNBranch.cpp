#include "Interpreter/AST/HTNBranch.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"

HTNBranch::HTNBranch(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNCondition>>& inConditions, const std::vector<std::shared_ptr<const HTNTask>>& inTasks)
	: mName(std::move(inName)), mConditions(inConditions), mTasks(inTasks)
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

bool HTNBranch::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	bool Result = true;

	// TODO Check all conditions, taking into acount and, not, and or
	for (const std::shared_ptr<const HTNCondition>& Condition : mConditions)
	{
		Result = Result && Condition->Check(ioDecompositionContext);
	}

	return Result;
}

std::string HTNBranch::GetName() const
{
	return mName ? mName->ToString() : "Invalid Branch";
}
