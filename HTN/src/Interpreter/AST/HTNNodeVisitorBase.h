#pragma once

#include <memory>
#include <vector>

class HTNBranch;
class HTNCondition;
class HTNDomain;
class HTNMethod;
class HTNPrimitiveTask;
class HTNTask;
class HTNValue;

class HTNNodeVisitorBase
{
public:
	virtual ~HTNNodeVisitorBase() = default;

	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNDomain& inDomain) const = 0;
	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNMethod& inMethod) const = 0;
	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNBranch& inBranch) const = 0;
	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNCondition& inCondition) const = 0;
	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNTask& inTask) const = 0;
	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNValue& inValue) const = 0;
};
