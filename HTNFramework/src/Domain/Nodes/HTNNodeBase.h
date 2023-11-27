// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

class HTNAtom;
class HTNNodeVisitorBase;
class HTNNodeVisitorContextBase;

/**
 * Base class for a node
 * - Uses the visitor pattern
 * @see HTNNodeVisitorBase
 */
class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = 0;

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    virtual void Accept(const HTNNodeVisitorBase& inNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Calls the 'Visit' member method overloaded for the node on the given node visitor with the given context
    HTN_NODISCARD virtual HTNAtom Accept(const HTNNodeVisitorBase& inNodeVisitor, HTNNodeVisitorContextBase& ioNodeVisitorContext) const;

    // Returns the ID of the node
    // - TODO salvarez Use string interning as an optimization
    HTN_NODISCARD virtual std::string GetID() const = 0;
};
