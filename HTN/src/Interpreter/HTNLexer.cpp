#include "Interpreter/HTNLexer.h"

#include "HTNToken.h"
#include "Log.h"

namespace
{
	bool IsValidCharacter(const char inCharacter)
	{
		return (inCharacter != '\0');
	}

	bool IsDigit(const char inCharacter)
	{
		return ((inCharacter >= '0') && (inCharacter <= '9'));
	}
}

bool HTNLexer::Lex(std::vector<HTNToken>& outTokens)
{
	bool Result = true;

	for (char Character = GetCharacter(); IsValidCharacter(Character); Character = GetCharacter())
	{
		//bool IsComment = false;

		switch (Character)
		{
		case '+':
		{
			// Plus
			AddToken(HTNTokenType::PLUS, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '-':
		{
			// Minus
			AddToken(HTNTokenType::MINUS, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '*':
		{
			// Multiplication
			AddToken(HTNTokenType::STAR, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '/':
		{
			// Division
			AddToken(HTNTokenType::SLASH, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '(':
		{
			// Left parenthesis
			AddToken(HTNTokenType::LEFT_PAREN, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case ')':
		{
			// Right parenthesis
			AddToken(HTNTokenType::RIGHT_PAREN, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		/*
		case '/':
		{
			// Comment
			const char NextCharacter = GetCharacter(1);
			if (NextCharacter != '/')
			{
				break;
			}

			IsComment = true;
			break;
		}
		*/
		case ' ':
		{
			// Whitespace
			AdvancePosition();
			break;
		}
		case '\n':
		{
			// New line
			AdvancePosition();
			++mLine;
			mColumn = 0;
			break;
		}
		default:
		{
			// Number
			const bool LexNumberResult = LexNumber(Character, outTokens);
			if (LexNumberResult)
			{
				break;
			}

			LOG_HTN_ERROR(mLine, mColumn, "Character {} not recognized", Character);
			AdvancePosition();
			Result = false;
			break;
		}
		}

		/*
		if (IsComment)
		{
			break;
		}
		*/
	}

	AddToken(HTNTokenType::END_OF_FILE, "", outTokens);

	return Result;
}

bool HTNLexer::LexNumber(const char inCharacter, std::vector<HTNToken>& outTokens)
{
	if (!IsDigit(inCharacter))
	{
		return false;
	}

	const int StartPosition = mPosition;

	AdvancePosition();

	// Check for more digits
	for (char Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
	{
		AdvancePosition();
	}

	// Check for fractional part
	char Character = GetCharacter();
	if (Character == '.')
	{
		const char NextCharacter = GetCharacter(1);
		if (IsDigit(NextCharacter))
		{
			AdvancePosition();

			// Check for more digits in fractional part
			for (Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
			{
				AdvancePosition();
			}
		}
	}

	const std::string Lexeme = mText.substr(StartPosition, (mPosition - StartPosition));
	const double Number = std::stod(Lexeme);
	AddToken(HTNTokenType::NUMBER, Lexeme, outTokens, Number);

	return true;
}

void HTNLexer::AddToken(const HTNTokenType inType, const std::string& inLexeme, std::vector<HTNToken>& outTokens, const std::any inValue) const
{
	outTokens.emplace_back(inType, inLexeme, mLine, mColumn, inValue);
}
