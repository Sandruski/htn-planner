#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

class HTNCondition final : public HTNNodeBase
{
public:
	explicit HTNCondition();
	~HTNCondition();

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

private:
	// TODO salvarez
};

