#include "Interpreter/HTNPrinter.h"

#include "Interpreter/Expressions/HTNBinaryExpression.h"
#include "Interpreter/Expressions/HTNLiteralExpression.h"
#include "Log.h"

bool HTNPrinter::Print(std::string& outText) const
{
	if (!mRoot)
	{
		return false;
	}

	std::any Value;
	const bool Result = mRoot->Accept(*this, Value);
	if (!Result)
	{
		return false;
	}

	outText = std::any_cast<std::string>(Value);

	return true;
}

bool HTNPrinter::Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outResult) const
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

	std::string Description = "(";
	Description.append(std::any_cast<std::string>(LeftValue));
	const std::string& OperatorName = inBinaryExpression.GetOperatorName();
	Description.append(OperatorName);
	Description.append(std::any_cast<std::string>(RightValue));
	Description.append(")");
	outResult = Description;

	return true;
}

bool HTNPrinter::Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outResult) const
{
	outResult = inLiteralExpression.GetName();

	return true;
}
