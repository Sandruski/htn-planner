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
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::PLUS, std::string(1, Character));
			break;
		}
		case '-':
		{
			// Minus
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::MINUS, std::string(1, Character));
			break;
		}
		case '*':
		{
			// Multiplication
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::STAR, std::string(1, Character));
			break;
		}
		case '/':
		{
			// Division
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::SLASH, std::string(1, Character));
			break;
		}
		case '(':
		{
			// Left parenthesis
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::LEFT_PAREN, std::string(1, Character));
			break;
		}
		case ')':
		{
			// Right parenthesis
			AdvancePosition();
			outTokens.emplace_back(HTNTokenType::RIGHT_PAREN, std::string(1, Character));
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
		default:
		{
			// Number
			const bool LexNumberResult = LexNumber(Character, outTokens);
			if (LexNumberResult)
			{
				break;
			}

			LOG("Character {} not recognized", Character);
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

	outTokens.emplace_back(HTNTokenType::END_OF_FILE, "");

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
	outTokens.emplace_back(HTNTokenType::NUMBER, Lexeme, Number);

	return true;
}
