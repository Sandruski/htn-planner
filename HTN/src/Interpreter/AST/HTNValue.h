#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNAtom.h"

enum class HTNValueType : unsigned int
{
    IDENTIFIER,
    AXIOM,
    VARIABLE,
    CONSTANT,
    LITERAL, // NUMBER OR STRING

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
    // - Identifier, variable, constant, axiom, or literal (number or string)
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
