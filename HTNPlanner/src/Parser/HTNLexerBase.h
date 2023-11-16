#pragma once

#include <string>
#include <unordered_map>

class HTNLexerContextBase;
enum class HTNTokenType : unsigned char;

/**
 * Breaks a text into a series of tokens
 * Reports all lexical errors of the text
 */
class HTNLexerBase
{
public:
    virtual ~HTNLexerBase() = 0;

protected:
    void LexIdentifier(const std::unordered_map<std::string, HTNTokenType>& inKeywords, HTNLexerContextBase& ioLexerContext) const;
    void LexNumber(HTNLexerContextBase& ioLexerContext) const;
    bool LexString(HTNLexerContextBase& ioLexerContext) const;
    void LexComment(HTNLexerContextBase& ioLexerContext) const;
};
