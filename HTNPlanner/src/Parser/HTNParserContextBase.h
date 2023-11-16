#pragma once

#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include <vector>

#if HTN_DEBUG
#include <string>
#endif

class HTNToken;

class HTNParserContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNParserContextBase(const std::vector<HTNToken>& inTokens);
    virtual ~HTNParserContextBase() = 0;

    const HTNToken* GetToken(const uint32 inPosition) const;
    std::size_t     GetTokensSize() const;

    void         SetPosition(const uint32 inPosition);
    void IncrementPosition();
    uint32 GetPosition() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::vector<HTNToken>& mTokens;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    uint32 mPosition = 0;

#if HTN_DEBUG
public:
    void               SetLastErrorMessage(const std::string& inLastErrorMessage);
    const std::string& GetLastErrorMessage() const;
    void               SetLastErrorRow(const int32 inLastErrorRow);
    int32                GetLastErrorRow() const;
    void               SetLastErrorColumn(const int32 inLastErrorColumn);
    int32                GetLastErrorColumn() const;

protected:
    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    std::string mLastErrorMessage;
    int32         mLastErrorRow    = -1;
    int32         mLastErrorColumn = -1;
#endif
};

inline void HTNParserContextBase::SetPosition(const uint32 inPosition)
{
    mPosition = inPosition;
}

inline void HTNParserContextBase::IncrementPosition()
{
    ++mPosition;
}

inline uint32 HTNParserContextBase::GetPosition() const
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

inline void HTNParserContextBase::SetLastErrorRow(const int32 inLastErrorRow)
{
    mLastErrorRow = inLastErrorRow;
}

inline int32 HTNParserContextBase::GetLastErrorRow() const
{
    return mLastErrorRow;
}

inline void HTNParserContextBase::SetLastErrorColumn(const int32 inLastErrorColumn)
{
    mLastErrorColumn = inLastErrorColumn;
}

inline int32 HTNParserContextBase::GetLastErrorColumn() const
{
    return mLastErrorColumn;
}
#endif
