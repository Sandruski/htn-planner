#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNCondition;
class HTNTask;
class HTNValue;

class HTNBranch final : public HTNNodeBase
{
public:
	explicit HTNBranch(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNCondition>>& inConditions, const std::vector<std::shared_ptr<const HTNTask>>& inTasks);
	~HTNBranch();

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::string GetName() const;
	const std::vector<std::shared_ptr<const HTNCondition>>& GetConditions() const;
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasks() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::vector<std::shared_ptr<const HTNCondition>> mConditions;
	std::vector<std::shared_ptr<const HTNTask>> mTasks;
};

inline const std::vector<std::shared_ptr<const HTNCondition>>& HTNBranch::GetConditions() const
{
	return mConditions;
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNBranch::GetTasks() const
{
	return mTasks;
}
