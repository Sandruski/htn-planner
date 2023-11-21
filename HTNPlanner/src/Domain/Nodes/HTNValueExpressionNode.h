// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"
#include "Domain/Nodes/HTNNodeBase.h"
#include "Helpers/HTNAtom.h"

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
    std::string    ToString() const;

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

    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;
};

/**
 * Node representing a literal expression
 */
class HTNLiteralExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNLiteralExpressionNode(const HTNAtom& inValue);

    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;
};

/**
 * Node representing a variable expression
 */
class HTNVariableExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNVariableExpressionNode(const HTNAtom& inValue);

    void    Accept(const HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;
};

/**
 * Node representing a constant expression
 */
class HTNConstantExpressionNode final : public HTNValueExpressionNodeBase
{
public:
    explicit HTNConstantExpressionNode(const HTNAtom& inValue);

    HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const final;
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
