#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNBranch;
class HTNValue;

class HTNMethod final : public HTNNodeBase
{
public:
	explicit HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNBranch>>& inBranches);
	~HTNMethod();

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::string GetName() const;
	const std::vector<std::shared_ptr<const HTNBranch>>& GetBranches() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::vector<std::shared_ptr<const HTNBranch>> mBranches;
};

inline const std::vector<std::shared_ptr<const HTNBranch>>& HTNMethod::GetBranches() const
{
	return mBranches;
}
