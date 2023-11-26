// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <string>
#include <vector>

class HTNAtom;
class HTNToken;
enum class HTNTokenType : uint8;

class HTNLexerContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNLexerContextBase(const std::string& inText, std::vector<HTNToken>& outTokens);
    virtual ~HTNLexerContextBase() = 0;

    HTN_NODISCARD const std::string& GetText() const;

    void AddToken(const HTNAtom& inValue, const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme));

    HTN_NODISCARD char GetCharacter(const uint32 inOffset = 0) const;

    void                 AdvancePosition(HTN_DEBUG_ONLY(const bool inIsNewLine = false));
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

#ifdef HTN_DEBUG
public:
    HTN_NODISCARD uint32 GetRow() const;
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

#ifdef HTN_DEBUG
inline uint32 HTNLexerContextBase::GetRow() const
{
    return mRow;
}

inline uint32 HTNLexerContextBase::GetColumn() const
{
    return mColumn;
}
#endif
