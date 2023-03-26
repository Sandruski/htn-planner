#include "Interpreter/HTNInterpreter.h"

#include "Interpreter/Expressions/HTNBinaryExpression.h"
#include "Interpreter/Expressions/HTNLiteralExpression.h"
#include "Log.h"

bool HTNInterpreter::Interpret(std::any& outValue) const
{
	if (!mRoot)
	{
		return false;
	}

	return mRoot->Accept(*this, outValue);
}

bool HTNInterpreter::Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outValue) const
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
	const bool LeftResult = LeftOperand->Accept(*this, LeftValue);
	if (!LeftResult)
	{
		LOG("Left result failed");
		return false;
	}

	std::any RightValue;
	const bool RightResult = RightOperand->Accept(*this, RightValue);
	if (!RightResult)
	{
		LOG("Right result failed");
		return false;
	}

	const HTNTokenType Operator = inBinaryExpression.GetOperatorType();
	switch (Operator)
	{
	case HTNTokenType::PLUS:
	{
		outValue = std::any_cast<double>(LeftValue) + std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::MINUS:
	{
		outValue = std::any_cast<double>(LeftValue) - std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::STAR:
	{
		outValue = std::any_cast<double>(LeftValue) * std::any_cast<double>(RightValue);
		break;
	}
	case HTNTokenType::SLASH:
	{
		outValue = std::any_cast<double>(LeftValue) / std::any_cast<double>(RightValue);
		break;
	}
	default:
	{
		LOG("Operator {} is invalid", static_cast<uint8_t>(Operator));
		return false;
	}
	}

	return true;
}

bool HTNInterpreter::Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outValue) const
{
	outValue = inLiteralExpression.GetValue();

	return true;
}
