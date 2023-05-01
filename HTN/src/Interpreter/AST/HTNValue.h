#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Interpreter/HTNToken.h"

class HTNValue final : public HTNNodeBase
{
public:
	explicit HTNValue(const HTNToken& inValue);

	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	std::string GetName() const;

private:
	HTNToken mValue;
};

inline HTNValue::HTNValue(const HTNToken& inValue)
	: mValue(inValue)
{
}
