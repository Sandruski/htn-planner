#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNMethod;
class HTNValue;

class HTNDomain final : public HTNNodeBase
{
public:
	explicit HTNDomain(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNMethod>>& inMethods);
	~HTNDomain();

	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::shared_ptr<const HTNMethod> FindMethodByName(const std::string& inMethodName) const;

	std::string GetName() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::vector<std::shared_ptr<const HTNMethod>> mMethods;
};
