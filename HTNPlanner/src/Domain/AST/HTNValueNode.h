#pragma once

#include "Domain/AST/HTNNodeBase.h"
#include "HTNAtom.h"

#include <string>

/**
 * Node representing a value
 */
class HTNValueNodeBase : public HTNNodeBase
{
public:
    explicit HTNValueNodeBase(const HTNAtom& inValue);

    std::string GetID() const final;

    const HTNAtom& GetValue() const;
    std::string    ToString() const;

private:
    // Value
    HTNAtom mValue;
};

/**
 * Node representing a value
 */
class HTNValueNode final : public HTNValueNodeBase
{
public:
    explicit HTNValueNode(const HTNAtom& inValue, const bool inIsIdentifier);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;

    bool IsIdentifier() const;

private:
    // Whether this node is an identifier
    bool mIsIdentifier = false;
};

/**
 * Node representing a variable value
 */
class HTNVariableValueNode final : public HTNValueNodeBase
{
public:
    explicit HTNVariableValueNode(const HTNAtom& inValue);

    void    Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const final;
    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;
};

/**
 * Node representing a constant value
 */
class HTNConstantValueNode final : public HTNValueNodeBase
{
public:
    explicit HTNConstantValueNode(const HTNAtom& inValue);

    HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor) const final;
};

inline HTNValueNodeBase::HTNValueNodeBase(const HTNAtom& inValue) : mValue(inValue)
{
}

inline std::string HTNValueNodeBase::GetID() const
{
    return "";
}

inline const HTNAtom& HTNValueNodeBase::GetValue() const
{
    return mValue;
}

inline HTNValueNode::HTNValueNode(const HTNAtom& inValue, const bool inIsIdentifier) : HTNValueNodeBase(inValue), mIsIdentifier(inIsIdentifier)
{
}

inline bool HTNValueNode::IsIdentifier() const
{
    return mIsIdentifier;
}

inline HTNVariableValueNode::HTNVariableValueNode(const HTNAtom& inValue) : HTNValueNodeBase(inValue)
{
}

inline HTNConstantValueNode::HTNConstantValueNode(const HTNAtom& inValue) : HTNValueNodeBase(inValue)
{
}
