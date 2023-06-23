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
	explicit HTNBranch(std::unique_ptr<const HTNValue> inName, const std::shared_ptr<const HTNConditionBase>& inPreCondition, const std::vector<std::shared_ptr<const HTNTask>>& inTasks);
	~HTNBranch();

	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::string GetName() const;
	const std::shared_ptr<const HTNConditionBase>& GetPreCondition() const;
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasks() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::shared_ptr<const HTNConditionBase> mPreCondition;
	std::vector<std::shared_ptr<const HTNTask>> mTasks;
};

inline const std::shared_ptr<const HTNConditionBase>& HTNBranch::GetPreCondition() const
{
	return mPreCondition;
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNBranch::GetTasks() const
{
	return mTasks;
}
