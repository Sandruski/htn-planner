#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDomain;
class HTNTask;
class HTNWorldState;

// Traverses an abstract syntax tree
class HTNInterpreter
{
public:
	explicit HTNInterpreter(const std::shared_ptr<const HTNDomain>& inDomain, const std::string& inRootCompoundTaskName);
	~HTNInterpreter();

	std::vector<std::shared_ptr<const HTNTask>> Interpret(const HTNWorldState& inWorldState);

private:
	std::shared_ptr<const HTNDomain> mDomain;
	std::string mRootCompoundTaskName;
};
