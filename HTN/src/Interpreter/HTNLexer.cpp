#include "Interpreter/HTNLexer.h"

#include "HTNLog.h"
#include "HTNToken.h"

#include <limits>
#include <unordered_map>

namespace
{
std::unordered_map<std::string, HTNTokenType> KeywordsToTokenTypes = {
    {"domain", HTNTokenType::HTN_DOMAIN}, {"top_level_domain", HTNTokenType::HTN_TOP_LEVEL_DOMAIN},
    {"method", HTNTokenType::HTN_METHOD}, {"top_level_method", HTNTokenType::HTN_TOP_LEVEL_METHOD},
    {"axiom", HTNTokenType::HTN_AXIOM},   {"constants", HTNTokenType::HTN_CONSTANTS},
    {"and", HTNTokenType::AND},           {"or", HTNTokenType::OR},
    {"alt", HTNTokenType::ALT},           {"not", HTNTokenType::NOT}};

std::unordered_map<char, std::string> SpecialCharactersToEscapeSequences = {{'\t', "\\t"}, {'\n', "\\n"}};

bool IsValidCharacter(const char inCharacter)
{
    return (inCharacter != '\0');
}

bool IsLetter(const char inCharacter)
{
    return (((inCharacter >= 'a') && (inCharacter <= 'z')) || ((inCharacter >= 'A') && (inCharacter <= 'Z')) ||
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
    const auto It = KeywordsToTokenTypes.find(inLexeme);
    return (It != KeywordsToTokenTypes.end() ? It->second : HTNTokenType::IDENTIFIER);
}

std::string GetSpecialCharacterEscapeSequence(const char inCharacter)
{
    const auto It = SpecialCharactersToEscapeSequences.find(inCharacter);
    return (It != SpecialCharactersToEscapeSequences.end() ? It->second : std::string(1, inCharacter));
}

} // namespace

bool HTNLexer::Lex(std::vector<HTNToken>& outTokens)
{
    bool Result = true;

    for (char Character = GetCharacter(); IsValidCharacter(Character); Character = GetCharacter())
    {
        switch (Character)
        {
        case ':': {
            // Colon
            AddToken(HTNTokenType::COLON, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '(': {
            // Left parenthesis
            AddToken(HTNTokenType::LEFT_PARENTHESIS, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case ')': {
            // Right parenthesis
            AddToken(HTNTokenType::RIGHT_PARENTHESIS, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '!': {
            // Exclamation mark
            AddToken(HTNTokenType::EXCLAMATION_MARK, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '?': {
            // Question mark
            AddToken(HTNTokenType::QUESTION_MARK, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '#': {
            // Hash
            AddToken(HTNTokenType::HASH, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '@': {
            // At
            AddToken(HTNTokenType::AT, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case ' ': {
            // Whitespace
            AdvancePosition();
            break;
        }
        case '\t': {
            // Tab
            AdvancePosition();
            break;
        }
        case '\n': {
            // Newline
            AdvancePosition(true);
            break;
        }
        case '/': {
            const char NextCharacter = GetCharacter(1);
            if (NextCharacter == '/')
            {
                // Comment
                LexComment();
                break;
            }

            LOG_HTN_ERROR(mRow, mColumn, "Expected '/' after [{}] for a comment", Character);
            Result = false;
            AdvancePosition();
            break;
        }
        case '"': {
            // String
            Result = LexString(outTokens) && Result;
            break;
        }
        default: {
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

            LOG_HTN_ERROR(mRow, mColumn, "Character [{}] not recognized", GetSpecialCharacterEscapeSequence(Character));
            Result = false;
            AdvancePosition();
        }
        }
    }

    AddToken(HTNTokenType::END_OF_FILE, "", HTNAtom(), outTokens);

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
    const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
    const HTNTokenType TokenType   = GetKeywordOrIdentifierTokenType(Lexeme);
    AddToken(TokenType, Lexeme, HTNAtom(Lexeme), outTokens);
}

void HTNLexer::LexNumber(std::vector<HTNToken>& outTokens)
{
    const unsigned int StartPosition = mPosition;

#ifdef HTN_DEBUG
    const unsigned int StartRow    = mRow;
    const unsigned int StartColumn = mColumn;
#endif

    AdvancePosition();

    // Check for more digits
    for (char Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
    {
        AdvancePosition();
    }

    // Check for fractional part
    const char NextCharacter = GetCharacter(1);
    if (GetCharacter() == '.' && IsDigit(NextCharacter))
    {
        AdvancePosition();

        // Check for more digits in fractional part
        for (char Character = GetCharacter(); IsDigit(Character); Character = GetCharacter())
        {
            AdvancePosition();
        }

        const unsigned int EndPosition = mPosition - StartPosition;
        const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
        const float        Number      = std::stof(Lexeme);
        CLOG_HTN_ERROR(std::stod(Lexeme) < std::numeric_limits<float>::min() || std::stod(Lexeme) > std::numeric_limits<float>::max(), StartRow,
                       StartColumn, "Number out of bounds");
        AddToken(HTNTokenType::NUMBER, Lexeme, HTNAtom(Number), outTokens);
    }
    else
    {
        const unsigned int EndPosition = mPosition - StartPosition;
        const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
        const int          Number      = std::stoi(Lexeme);
        AddToken(HTNTokenType::NUMBER, Lexeme, HTNAtom(Number), outTokens);
    }
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

    const unsigned int EndPosition = mPosition - StartPosition;

    AdvancePosition();

    const std::string Lexeme = mText.substr(StartPosition + 1, EndPosition - 1);
    AddToken(HTNTokenType::STRING, Lexeme, HTNAtom(Lexeme), outTokens);

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

void HTNLexer::AddToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, std::vector<HTNToken>& outTokens) const
{
    outTokens.emplace_back(inType, inLexeme, inValue, mRow, mColumn);
}
