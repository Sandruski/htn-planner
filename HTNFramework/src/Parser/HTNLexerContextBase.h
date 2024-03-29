// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <string>
#include <vector>

class HTNAtom;
class HTNToken;
enum class HTNTokenType : uint8;

/**
 * Base class for the contextual data of a lexer
 */
class HTNLexerContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNLexerContextBase(const std::string& inText, std::vector<HTNToken>& outTokens);
    virtual ~HTNLexerContextBase() = 0;

    // Returns the text
    HTN_NODISCARD const std::string& GetText() const;

    // Adds a new token
    void AddToken(const HTNAtom& inValue, const HTNTokenType inType HTN_LOG_ONLY(, const std::string& inLexeme));

    // Returns the character at the position plus the given offset
    HTN_NODISCARD char GetCharacter(const uint32 inOffset = 0) const;

    // Increments the position
    void AdvancePosition(HTN_LOG_ONLY(const bool inIsNewLine = false));

    // Returns the position
    HTN_NODISCARD uint32 GetPosition() const;

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
    uint32 mPosition = 0;

#ifdef HTN_ENABLE_LOGGING
public:
    // Returns the row
    HTN_NODISCARD uint32 GetRow() const;

    // Returns the column
    HTN_NODISCARD uint32 GetColumn() const;

private:
    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    uint32 mRow    = 0;
    uint32 mColumn = 0;
#endif
};

inline const std::string& HTNLexerContextBase::GetText() const
{
    return mText;
}

inline uint32 HTNLexerContextBase::GetPosition() const
{
    return mPosition;
}

#ifdef HTN_ENABLE_LOGGING
inline uint32 HTNLexerContextBase::GetRow() const
{
    return mRow;
}

inline uint32 HTNLexerContextBase::GetColumn() const
{
    return mColumn;
}
#endif
