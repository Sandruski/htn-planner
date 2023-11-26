// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "Domain/Nodes/HTNNodeBase.h"
#include "HTNCoreMinimal.h"

#include <string>

/**
 * Node representing a value expression
 */
class HTNValueExpressionNodeBase : public HTNNodeBase
{
public:
    explicit HTNValueExpressionNodeBase(const HTNAtom& inValue);

    HTN_NODISCARD std::string GetID() const final;

    HTN_NODISCARD const HTNAtom& GetValue() const;

protected:
    // Value
    HTNAtom mValue;
};

/**
 * Node representing an identifier expression
 */
class HTNIdentifierExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNIdentifierExpressionNode(const HTNAtom& inValue);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a literal expression
 */
class HTNLiteralExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNLiteralExpressionNode(const HTNAtom& inValue);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a variable expression
 */
class HTNVariableExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNVariableExpressionNode(const HTNAtom& inValue);

    void Accept(const HTNNodeVisitorBase& inNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a constant expression
 */
class HTNConstantExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNConstantExpressionNode(const HTNAtom& inValue);

    HTN_NODISCARD HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

inline std::string HTNValueExpressionNodeBase::GetID() const
{
    assert(false && "Value expression nodes do not have ID");
    return "";
}

inline const HTNAtom& HTNValueExpressionNodeBase::GetValue() const
{
    return mValue;
}
