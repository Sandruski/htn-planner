#include "Interpreter/HTNLexer.h"

#include "HTNToken.h"
#include "Log.h"

#include <unordered_map>

namespace
{
	std::unordered_map<std::string, HTNTokenType> Keywords = {
	{ "domain", HTNTokenType::HTN_DOMAIN },
	{ "method", HTNTokenType::HTN_METHOD },
	{ "and", HTNTokenType::AND },
	{ "or", HTNTokenType::OR },
	{ "not", HTNTokenType::NOT }
	};

	bool IsValidCharacter(const char inCharacter)
	{
		return (inCharacter != '\0');
	}

	bool IsLetter(const char inCharacter)
	{
		return (((inCharacter >= 'a') && (inCharacter <= 'z')) ||
			((inCharacter >= 'A') && (inCharacter <= 'Z')) ||
			(inCharacter >= '_')); // Underscore is considered a letter
	}

	bool IsDigit(const char inCharacter)
	{
		return ((inCharacter >= '0') && (inCharacter <= '9'));
	}

	bool IsAlphaNumeric(const char inCharacter)
	{
		return (IsLetter(inCharacter) || IsDigit(inCharacter));
	}

	HTNTokenType GetKeywordOrIdentifierTokenType(const std::string& inLexeme)
	{
		const auto It = Keywords.find(inLexeme);
		return (It != Keywords.end() ? It->second : HTNTokenType::IDENTIFIER);
	}
}

bool HTNLexer::Lex(std::vector<HTNToken>& outTokens)
{
	bool Result = true;

	for (char Character = GetCharacter(); IsValidCharacter(Character); Character = GetCharacter())
	{
		switch (Character)
		{
		case ':':
		{
			// Colon
			AddToken(HTNTokenType::COLON, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '(':
		{
			// Left parenthesis
			AddToken(HTNTokenType::LEFT_PARENTHESIS, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case ')':
		{
			// Right parenthesis
			AddToken(HTNTokenType::RIGHT_PARENTHESIS, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '!':
		{
			// Exclamation mark
			AddToken(HTNTokenType::EXCLAMATION_MARK, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '?':
		{
			// Question mark
			AddToken(HTNTokenType::QUESTION_MARK, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case '#':
		{
			// Hash
			AddToken(HTNTokenType::HASH, std::string(1, Character), outTokens);
			AdvancePosition();
			break;
		}
		case ' ':
		{
			// Whitespace
			AdvancePosition();
			break;
		}
		case '\t':
		{
			// Tab
			AdvancePosition();
			break;
		}
		case '\n':
		{
			// Newline
			AdvancePosition(true);
			break;
		}
		case '/':
		{
			const char NextCharacter = GetCharacter(1);
			if (NextCharacter == '/')
			{
				// Comment
				LexComment();
				break;
			}

			LOG_HTN_ERROR(mRow, mColumn, "Expected '/' after '{}' for a comment", Character);
			Result = false;
			AdvancePosition();
			break;
		}
		case '"':
		{
			// String
			Result = Result && LexString(outTokens);
			break;
		}
		default:
		{
			if (IsDigit(Character))
			{
				// Number
				LexNumber(outTokens);
				break;
			}
			else if (IsLetter(Character))
			{
				// Keyword or identifier
				LexKeywordOrIdentifier(outTokens);
				break;
			}

			// TODO jose If the character contains a '\', convert it to a '\\' so it is displayed correctly
			LOG_HTN_ERROR(mRow, mColumn, "Character '{}' not recognized", Character);
			Result = false;
			AdvancePosition();
		}
		}
	}

	AddToken(HTNTokenType::END_OF_FILE, "", outTokens);

	return Result;
}

void HTNLexer::LexKeywordOrIdentifier(std::vector<HTNToken>& outTokens)
{
	const unsigned int StartPosition = mPosition;

	AdvancePosition();

	for (char Character = GetCharacter(); IsAlphaNumeric(Character); Character = GetCharacter())
	{
		AdvancePosition();
	}

	const unsigned int EndPosition = mPosition - StartPosition;
	const std::string Lexeme = mText.substr(StartPosition, EndPosition);
	const HTNTokenType TokenType = GetKeywordOrIdentifierTokenType(Lexeme);
	AddToken(TokenType, Lexeme, outTokens, Lexeme);
}

void HTNLexer::LexNumber(std::vector<HTNToken>& outTokens)
{
	const unsigned int StartPosition = mPosition;

	AdvancePosition();

	// Check for more digits
	for (char Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
	{
		AdvancePosition();
	}

	// Check for fractional part
	if (GetCharacter() == '.')
	{
		const char NextCharacter = GetCharacter(1);
		if (IsDigit(NextCharacter))
		{
			AdvancePosition();

			// Check for more digits in fractional part
			for (char Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
			{
				AdvancePosition();
			}
		}
	}

	const unsigned int EndPosition = mPosition - StartPosition;
	const std::string Lexeme = mText.substr(StartPosition + 1, EndPosition - 1);
	const double Number = std::stod(Lexeme);
	AddToken(HTNTokenType::NUMBER, Lexeme, outTokens, Number);
}

bool HTNLexer::LexString(std::vector<HTNToken>& outTokens)
{
	const unsigned int StartPosition = mPosition;

	AdvancePosition();

	for (char Character = GetCharacter(); Character != '"'; Character = GetCharacter())
	{
		AdvancePosition();
	}

	if (GetCharacter() != '"')
	{
		LOG_HTN_ERROR(mRow, mColumn, "Character '\"' not found at end of string");
		return false;
	}

	AdvancePosition();

	const unsigned int EndPosition = mPosition - StartPosition;
	const std::string Lexeme = mText.substr(StartPosition, EndPosition);
	AddToken(HTNTokenType::STRING, Lexeme, outTokens, Lexeme);

	return true;
}

void HTNLexer::LexComment()
{
	for (char Character = GetCharacter(); Character != '\n'; Character = GetCharacter())
	{
		AdvancePosition();
	}

	AdvancePosition(true);
}

void HTNLexer::AddToken(const HTNTokenType inType, const std::string& inLexeme, std::vector<HTNToken>& outTokens, const std::any inValue) const
{
	outTokens.emplace_back(inType, inLexeme, mRow, mColumn, inValue);
}
