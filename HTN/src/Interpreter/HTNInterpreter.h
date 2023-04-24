#pragma once

#include "Interpreter/AST/HTNNodeVisitorBase.h"

#include <memory>
#include <string>

class HTNDomain;
class HTNPrimitiveTask;
class HTNWorldState;

// Traverses an abstract syntax tree
class HTNInterpreter final : public HTNNodeVisitorBase
{
public:
	explicit HTNInterpreter(const std::shared_ptr<const HTNDomain>& inDomain, const std::string& inRootCompoundTaskName);
	~HTNInterpreter();

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Interpret(const HTNWorldState& inWorldState) const;

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNDomain& inDomain) const final;
	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNMethod& inMethod) const final;
	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNBranch& inBranch) const final;
	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNCondition& inCondition) const final;
	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNTask& inTask) const final;
	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Visit(const HTNValue& inValue) const final;

private:
	std::shared_ptr<const HTNDomain> mDomain;
	std::string mRootCompoundTaskName;
};
