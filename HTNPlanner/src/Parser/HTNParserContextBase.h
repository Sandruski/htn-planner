#pragma once

#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "Helpers/HTNToken.h"

#include <vector>

#if HTN_DEBUG
#include <string>
#endif

class HTNParserContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNParserContextBase(const std::vector<HTNToken>& inTokens);
    virtual ~HTNParserContextBase() = 0;

    const HTNToken* GetToken(const unsigned int inPosition) const;
    std::size_t     GetTokensSize() const;

    void         SetPosition(const unsigned int inPosition);
    void IncrementPosition();
    unsigned int GetPosition() const;

private:
    std::vector<HTNToken> mTokens;

    unsigned int mPosition = 0;

#if HTN_DEBUG
public:
    void               SetLastErrorMessage(const std::string& inLastErrorMessage);
    const std::string& GetLastErrorMessage() const;
    void               SetLastErrorRow(const int inLastErrorRow);
    int                GetLastErrorRow() const;
    void               SetLastErrorColumn(const int inLastErrorColumn);
    int                GetLastErrorColumn() const;

protected:
    std::string mLastErrorMessage;
    int         mLastErrorRow    = -1;
    int         mLastErrorColumn = -1;
#endif
};

inline const HTNToken* HTNParserContextBase::GetToken(const unsigned int inPosition) const
{
    return ((inPosition < mTokens.size()) ? &mTokens[inPosition] : nullptr);
}

inline std::size_t HTNParserContextBase::GetTokensSize() const
{
    return mTokens.size();
}

inline void HTNParserContextBase::SetPosition(const unsigned int inPosition)
{
    mPosition = inPosition;
}

inline void HTNParserContextBase::IncrementPosition()
{
    ++mPosition;
}

inline unsigned int HTNParserContextBase::GetPosition() const
{
    return mPosition;
}

#if HTN_DEBUG
inline void HTNParserContextBase::SetLastErrorMessage(const std::string& inLastErrorMessage)
{
    mLastErrorMessage = inLastErrorMessage;
}

inline const std::string& HTNParserContextBase::GetLastErrorMessage() const
{
    return mLastErrorMessage;
}

inline void HTNParserContextBase::SetLastErrorRow(const int inLastErrorRow)
{
    mLastErrorRow = inLastErrorRow;
}

inline int HTNParserContextBase::GetLastErrorRow() const
{
    return mLastErrorRow;
}

inline void HTNParserContextBase::SetLastErrorColumn(const int inLastErrorColumn)
{
    mLastErrorColumn = inLastErrorColumn;
}

inline int HTNParserContextBase::GetLastErrorColumn() const
{
    return mLastErrorColumn;
}
#endif
