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
	~HTNInterpreter();

	bool Init(const std::string& inDomainPath);

	std::vector<std::shared_ptr<const HTNTask>> Interpret(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const;

private:
	std::shared_ptr<const HTNDomain> mDomain;
};
