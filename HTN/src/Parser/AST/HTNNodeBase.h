#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNNodeVisitorBase;
class HTNTaskInstance;
class HTNTaskNode;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual std::vector<HTNTaskInstance> Accept(const HTNNodeVisitorBase& inNodeVisitor) const = 0;
    virtual std::string                  GetID() const                                         = 0;
    virtual std::string                  ToString() const                                      = 0;
};
