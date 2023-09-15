#pragma once

#include "HTNAtom.h"
#include "Parser/AST/HTNNodeBase.h"

enum class HTNValueType : unsigned int
{
    SYMBOL,
    AXIOM,
    VARIABLE,
    CONSTANT,
    LITERAL,

    INVALID
};

/**
 * Node representing a value
 */
class HTNValueNode final : public HTNNodeBase
{
public:
    explicit HTNValueNode(const HTNAtom& inValue, const HTNValueType inType);

    void        Accept(const HTNNodeVisitorBase& inNodeVisitor) const final;
    std::string GetID() const final;
    std::string ToString() const final;

    const HTNAtom& GetValue() const;
    HTNValueType   GetType() const;

private:
    // Value
    HTNAtom mValue;

    // Type of the value
    // - Symbol, variable, constant, axiom, or literal
    HTNValueType mType = HTNValueType::INVALID;
};

inline HTNValueNode::HTNValueNode(const HTNAtom& inValue, const HTNValueType inType) : mValue(inValue), mType(inType)
{
}

inline const HTNAtom& HTNValueNode::GetValue() const
{
    return mValue;
}

inline HTNValueType HTNValueNode::GetType() const
{
    return mType;
}
