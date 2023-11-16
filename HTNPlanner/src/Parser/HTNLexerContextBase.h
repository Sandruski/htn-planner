#pragma once

#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Helpers/HTNMacros.h"

#include <string>
#include <vector>

class HTNAtom;
class HTNToken;
enum class HTNTokenType : unsigned int;

class HTNLexerContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNLexerContextBase(const std::string& inText, std::vector<HTNToken>& outTokens);
    virtual ~HTNLexerContextBase() = 0;

    const std::string& GetText() const;

    void AddToken(const HTNAtom& inValue, const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme));

    char GetCharacter(const unsigned int inLookAhead = 0) const;

    void         AdvancePosition(const bool inIsNewLine = false);
    unsigned int GetPosition() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::string& mText;

    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    std::vector<HTNToken>& mTokens;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    unsigned int mPosition = 0;

#ifdef HTN_DEBUG
public:
    unsigned int GetRow() const;
    unsigned int GetColumn() const;

private:
    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    unsigned int mRow    = 0;
    unsigned int mColumn = 0;
#endif
};

inline const std::string& HTNLexerContextBase::GetText() const
{
    return mText;
}

inline unsigned int HTNLexerContextBase::GetPosition() const
{
    return mPosition;
}

#ifdef HTN_DEBUG
inline unsigned int HTNLexerContextBase::GetRow() const
{
    return mRow;
}

inline unsigned int HTNLexerContextBase::GetColumn() const
{
    return mColumn;
}
#endif
