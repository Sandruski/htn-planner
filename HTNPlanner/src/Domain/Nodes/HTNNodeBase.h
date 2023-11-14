#pragma once

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
    virtual ~HTNNodeBase() = default;

    virtual void    Accept(HTNNodeVisitorBase& ioNodeVisitor, const HTNAtom& inNodeValue, HTNNodeVisitorContextBase& ioContext) const;
    virtual HTNAtom Accept(HTNNodeVisitorBase& ioNodeVisitor, HTNNodeVisitorContextBase& ioContext) const;

    virtual std::string GetID() const    = 0;
};
