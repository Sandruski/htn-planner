#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNNodeVisitorBase;
class HTNPrimitiveTask;

class HTNNodeBase
{
public:
	virtual ~HTNNodeBase() = default;

	virtual std::vector<std::shared_ptr<const HTNPrimitiveTask>> Accept(const HTNNodeVisitorBase& inVisitor) const = 0;
	virtual std::string ToString() const = 0;
};
