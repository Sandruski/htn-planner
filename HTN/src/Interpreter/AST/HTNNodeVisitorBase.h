#pragma once

#include <memory>
#include <vector>

class HTNBranch;
class HTNConditionBase;
class HTNDomain;
class HTNMethod;
class HTNTask;
class HTNTask;
class HTNValue;
class HTNAxiom;
class HTNConstant;
class HTNConstants;

// TODO salvarez Revisit this class and implement planning and printing the AST tree through it
class HTNNodeVisitorBase
{
public:
    virtual ~HTNNodeVisitorBase() = default;

    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNDomain& inDomain) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNMethod& inMethod) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNBranch& inBranch) const           = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNConditionBase& inCondition) const = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNTask& inTask) const               = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNValue& inValue) const             = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNAxiom& inValue) const             = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNConstant& inValue) const          = 0;
    virtual std::vector<std::shared_ptr<const HTNTask>> Visit(const HTNConstants& inValue) const         = 0;
};
