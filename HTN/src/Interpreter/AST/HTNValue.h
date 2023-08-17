#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNAtom.h"

class HTNValue final : public HTNNodeBase
{
public:
    explicit HTNValue(const HTNAtom& inValue, const bool inIsIdentifier);

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    const HTNAtom& GetValue() const;
    bool           IsIdentifier() const;

private:
    HTNAtom mValue;
    bool    mIsIdentifier = false;
};

inline HTNValue::HTNValue(const HTNAtom& inValue, const bool inIsIdentifier) : mValue(inValue), mIsIdentifier(inIsIdentifier)
{
}

inline const HTNAtom& HTNValue::GetValue() const
{
    return mValue;
}

inline bool HTNValue::IsIdentifier() const
{
    return mIsIdentifier;
}
