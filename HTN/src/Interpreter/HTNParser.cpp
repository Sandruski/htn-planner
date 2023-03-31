#include "Interpreter/HTNParser.h"

#include "Interpreter/Expressions/HTNBinaryExpression.h"
#include "Interpreter/Expressions/HTNLiteralExpression.h"
#include "Log.h"

#include <initializer_list>

namespace
{
	bool IsTokenType(const HTNToken* inToken, const std::initializer_list<HTNTokenType>& inTokenTypes)
	{
		if (!inToken)
		{
			return false;
		}

		const HTNTokenType TokenType = inToken->GetType();
		const auto It = std::find(inTokenTypes.begin(), inTokenTypes.end(), TokenType);
		return (It != inTokenTypes.end());
	}
}

bool HTNParser::Parse(std::unique_ptr<const HTNExpressionBase>& outRoot)
{
	return Expression(outRoot);
}

bool HTNParser::Expression(std::unique_ptr<const HTNExpressionBase>& outNode)
{
	return Term(outNode);
}

bool HTNParser::Term(std::unique_ptr<const HTNExpressionBase>& outNode)
{
	// factor ((PLUS | MINUS) factor)*

	std::unique_ptr<const HTNExpressionBase> Left;
	const bool LeftResult = Factor(Left);
	if (!LeftResult)
	{
		return false;
	}

	for (const HTNToken* Token = GetToken(); IsTokenType(Token, { HTNTokenType::PLUS , HTNTokenType::MINUS }); Token = GetToken())
	{
		AdvancePosition();

		std::unique_ptr<const HTNExpressionBase> Right;
		const bool RightResult = Factor(Right);
		if (!RightResult)
		{
			return false;
		}

		Left = std::make_unique<HTNBinaryExpression>(std::move(Left), *Token, std::move(Right));
	}

	outNode = std::move(Left);

	return true;
}

bool HTNParser::Factor(std::unique_ptr<const HTNExpressionBase>& outNode)
{
	// primary ((MUL | DIV) primary)*

	std::unique_ptr<const HTNExpressionBase> Left;
	const bool LeftResult = Primary(Left);
	if (!LeftResult)
	{
		return false;
	}

	for (const HTNToken* Token = GetToken(); IsTokenType(Token, { HTNTokenType::STAR , HTNTokenType::SLASH }); Token = GetToken())
	{
		AdvancePosition();

		std::unique_ptr<const HTNExpressionBase> Right;
		const bool RightResult = Primary(Right);
		if (!RightResult)
		{
			return false;
		}

		Left = std::make_unique<HTNBinaryExpression>(std::move(Left), *Token, std::move(Right)); 
	}

	outNode = std::move(Left);

	return true;
}

bool HTNParser::Primary(std::unique_ptr<const HTNExpressionBase>& outNode)
{
	// NUMBER

	const HTNToken* Token = GetToken();
	if (IsTokenType(Token, { HTNTokenType::NUMBER }))
	{
		AdvancePosition();

		outNode = std::make_unique<HTNLiteralExpression>(*Token);
		return true;
	}
	else if (IsTokenType(Token, { HTNTokenType::LEFT_PAREN }))
	{
		AdvancePosition();

		const bool Result = Expression(outNode);
		if (!Result)
		{
			return false;
		}

		Token = GetToken();
		if (!IsTokenType(Token, { HTNTokenType::RIGHT_PAREN }))
		{
#ifdef HTN_DEBUG
			if (Token)
			{
				LOG_HTN_ERROR(Token->GetLine(), Token->GetColumn(), "Token type is not a right parenthesis");
			}
#endif
			return false;
		}

		AdvancePosition();

		return true;
	}

#ifdef HTN_DEBUG
	if (Token)
	{
		LOG_HTN_ERROR(Token->GetLine(), Token->GetColumn(), "Token type is not a number or a left parenthesis");
	}
#endif

	return false;
}
