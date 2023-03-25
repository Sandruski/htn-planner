#pragma once

#include "Interpreter/HTNToken.h"

#include <any>

class HTNExpressionVisitorBase;

class HTNExpressionBase
{
public:
	explicit HTNExpressionBase(const HTNToken& inToken);

	virtual [[nodiscard]] bool Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outResult) const = 0;

protected:
	HTNToken mToken;
};

inline HTNExpressionBase::HTNExpressionBase(const HTNToken& inToken)
	: mToken(inToken)
{
}
