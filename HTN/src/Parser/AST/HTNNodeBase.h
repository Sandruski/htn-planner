#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNNodeVisitorBase;
class HTNTaskNode;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual std::vector<std::shared_ptr<const HTNTaskNode>> Accept(const HTNNodeVisitorBase& inNodeVisitor) const = 0;
    virtual std::string                                     GetID() const                                         = 0;
    virtual std::string                                     ToString() const                                      = 0;
};
