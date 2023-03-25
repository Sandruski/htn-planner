#include "Interpreter/HTNInterpreter.h"

#include "Interpreter/Expressions/HTNBinaryExpression.h"
#include "Interpreter/Expressions/HTNExpressionBase.h"
#include "Interpreter/Expressions/HTNLiteralExpression.h"
#include "Log.h"

#include <cassert>

bool HTNInterpreter::Interpret(std::any& outResult) const
{
	return (mRoot ? mRoot->Accept(*this, outResult) : false);
}

bool HTNInterpreter::Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outResult) const
{
	const HTNExpressionBase* LeftOperand = inBinaryExpression.GetLeftOperand();
	if (!LeftOperand)
	{
		LOG("Left operand is null");
		return false;
	}

	const HTNExpressionBase* RightOperand = inBinaryExpression.GetRightOperand();
	if (!RightOperand)
	{
		LOG("Right operand is null");
		return false;
	}

	std::any LeftValue;
	bool Result = LeftOperand->Accept(*this, LeftValue);
	std::any RightValue;
	Result = Result && RightOperand->Accept(*this, RightValue);

	const HTNTokenType Operator = inBinaryExpression.GetOperator();
	switch (Operator)
	{
	case HTNTokenType::PLUS:
	{
		outResult = std::any_cast<double>(LeftValue) + std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::MINUS:
	{
		outResult = std::any_cast<double>(LeftValue) - std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::STAR:
	{
		outResult = std::any_cast<double>(LeftValue) * std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::SLASH:
	{
		outResult = std::any_cast<double>(LeftValue) / std::any_cast<double>(RightValue);
		break;
	}
	default:
	{
		LOG("Operator {} is invalid", static_cast<uint8_t>(Operator));
		return false;
	}
	}

	return Result;
}

bool HTNInterpreter::Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outResult) const
{
	outResult = inLiteralExpression.GetValue();

	return true;
}
