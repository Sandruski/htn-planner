#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNDecompositionContext;
class HTNTask;
class HTNValue;

class HTNBranch final : public HTNNodeBase
{
public:
	explicit HTNBranch(std::unique_ptr<const HTNValue> inName, const std::shared_ptr<const HTNConditionBase>& inCondition, const std::vector<std::shared_ptr<const HTNTask>>& inTasks);
	~HTNBranch();

	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::string GetName() const;
	const std::shared_ptr<const HTNConditionBase>& GetCondition() const;
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasks() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::shared_ptr<const HTNConditionBase> mCondition;
	std::vector<std::shared_ptr<const HTNTask>> mTasks;
};

inline const std::shared_ptr<const HTNConditionBase>& HTNBranch::GetCondition() const
{
	return mCondition;
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNBranch::GetTasks() const
{
	return mTasks;
}
