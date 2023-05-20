#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionContext;
class HTNDomain;
class HTNTask;

// Traverses an abstract syntax tree
class HTNInterpreter
{
public:
	~HTNInterpreter();

	bool Init(const std::string& inDomainPath);

	std::vector<std::shared_ptr<const HTNTask>> Interpret(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const;

private:
	std::shared_ptr<const HTNDomain> mDomain;
};
