// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

class HTNAtom;
class HTNNodeVisitorBase;
class HTNNodeVisitorContextBase;

/**
 * Visitor pattern
 * @see HTNNodeVisitorBase
 */
class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = 0;

    virtual void Accept(const HTNNodeVisitorBase& inNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;
    HTN_NODISCARD virtual HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // TODO salvarez Use string interning as an optimization
    HTN_NODISCARD virtual std::string GetID() const = 0;
};
