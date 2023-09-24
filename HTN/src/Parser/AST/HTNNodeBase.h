#pragma once

#include "HTNAtom.h"

#include <string>

class HTNNodeVisitorBase;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual void        Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue) const;
    virtual HTNAtom     Accept(HTNNodeVisitorBase& ioNodeVisitor) const;
    virtual std::string GetID() const                                                           = 0;
    virtual std::string ToString() const                                                        = 0;
};
