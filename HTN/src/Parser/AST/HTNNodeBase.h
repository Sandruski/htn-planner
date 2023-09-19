#pragma once

#include "HTNAtom.h"

#include <string>

class HTNNodeVisitorBase;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual HTNAtom     Accept(HTNNodeVisitorBase& inNodeVisitor) const = 0;
    virtual std::string GetID() const                                   = 0;
    virtual std::string ToString() const                                = 0;
};
