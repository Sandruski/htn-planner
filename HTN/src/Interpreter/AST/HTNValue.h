#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNAtom.h"

class HTNValue final : public HTNNodeBase
{
public:
    explicit HTNValue(const HTNAtom& inValue);

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    const HTNAtom& GetValue() const;

private:
    HTNAtom mValue;
};

inline HTNValue::HTNValue(const HTNAtom& inValue) : mValue(inValue)
{
}

inline const HTNAtom& HTNValue::GetValue() const
{
    return mValue;
}
