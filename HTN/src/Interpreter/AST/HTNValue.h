#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNAtom.h"

enum class HTNValueType : unsigned int
{
    SYMBOL,
    AXIOM,
    VARIABLE,
    CONSTANT,
    LITERAL,

    INVALID
};

class HTNValue final : public HTNNodeBase
{
public:
    explicit HTNValue(const HTNAtom& inValue, const HTNValueType inType);

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const HTNAtom& GetValue() const;
    HTNValueType   GetType() const;

private:
    // Value
    HTNAtom mValue;

    // Type of the value
    // - Symbol, variable, constant, axiom, or literal
    HTNValueType mType = HTNValueType::INVALID;
};

inline HTNValue::HTNValue(const HTNAtom& inValue, const HTNValueType inType) : mValue(inValue), mType(inType)
{
}

inline const HTNAtom& HTNValue::GetValue() const
{
    return mValue;
}

inline HTNValueType HTNValue::GetType() const
{
    return mType;
}
