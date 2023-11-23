// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"
#include "Core/HTNAtom.h"

#include <string>

/**
 * Node representing a value expression
 */
class HTNValueExpressionNodeBase : public HTNNodeBase
{
public:
    explicit HTNValueExpressionNodeBase(const HTNAtom& inValue);

    std::string GetID() const final;

    const HTNAtom& GetValue() const;

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

    HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a literal expression
 */
class HTNLiteralExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNLiteralExpressionNode(const HTNAtom& inValue);

    HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a variable expression
 */
class HTNVariableExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNVariableExpressionNode(const HTNAtom& inValue);

    void    Accept(const HTNNodeVisitorBase& inNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
    HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
};

/**
 * Node representing a constant expression
 */
class HTNConstantExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNConstantExpressionNode(const HTNAtom& inValue);

    HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const final;
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
