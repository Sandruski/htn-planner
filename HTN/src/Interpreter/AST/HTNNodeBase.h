#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNNodeVisitorBase;
class HTNTask;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const = 0;
    virtual std::string                                 GetID() const                                     = 0;
    virtual std::string                                 ToString() const                                  = 0;
};
