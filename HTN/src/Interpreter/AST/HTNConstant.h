#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNValue;

class HTNConstant final : public HTNNodeBase
{
public:
    explicit HTNConstant(const std::vector<std::shared_ptr<const HTNValue>>& inArguments);
    ~HTNConstant();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::vector<std::shared_ptr<const HTNValue>>& GetArguments() const;
    const std::shared_ptr<const HTNValue>&              GetIDArgument() const;
    const std::shared_ptr<const HTNValue>&              GetValueArgument() const;

private:
    // Arguments of the constant
    // - 1: ID of the constant, which is unique within its group of constants
    // - 2: value of the constant
    std::vector<std::shared_ptr<const HTNValue>> mArguments;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNConstant::GetArguments() const
{
    return mArguments;
}

inline const std::shared_ptr<const HTNValue>& HTNConstant::GetIDArgument() const
{
    return mArguments[0];
}

inline const std::shared_ptr<const HTNValue>& HTNConstant::GetValueArgument() const
{
    return mArguments[1];
}
