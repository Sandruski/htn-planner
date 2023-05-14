#pragma once

#include <memory>
#include <vector>

class HTNBranch;
class HTNConditionBase;
class HTNDomain;
class HTNMethod;
class HTNTask;
class HTNTask;
class HTNValue;

class HTNNodeVisitorBase
{
public:
	virtual ~HTNNodeVisitorBase() = default;

	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNDomain& inDomain) const = 0;
	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNMethod& inMethod) const = 0;
	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNBranch& inBranch) const = 0;
	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNConditionBase& inCondition) const = 0;
	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNTask& inTask) const = 0;
	virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNValue& inValue) const = 0;
};