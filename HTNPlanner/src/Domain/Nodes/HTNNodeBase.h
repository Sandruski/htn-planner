// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"

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

    virtual void    Accept(const HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Accept(const HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const;

    virtual std::string GetID() const = 0;
};
