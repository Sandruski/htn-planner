#pragma once

/*
#include "Interpreter/AST/HTNNodeVisitorBase.h"

#include <memory>
#include <string>

class HTNDomain;

// Returns a string representing the abstract syntax tree of the expression
// Only used for debug purposes
class HTNPrinter final : public HTNNodeVisitorBase
{
public:
	explicit HTNPrinter(std::unique_ptr<const HTNDomain> inDomain);
	~HTNPrinter();

	std::string Print() const;

	bool Visit(const HTNDomain& inDomain, std::any& outValue) const final;
	bool Visit(const HTNMethod& inMethod, std::any& outValue) const final;
	bool Visit(const HTNBranch& inBranch, std::any& outValue) const final;
	bool Visit(const HTNCondition& inCondition, std::any& outValue) const final;
	bool Visit(const HTNPrimitiveTask& inTask, std::any& outValue) const final;
	bool Visit(const HTNValue& inValue, std::any& outValue) const final;

private:
	std::unique_ptr<const HTNDomain> mDomain;
};
*/