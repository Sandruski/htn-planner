#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>

class HTNConditionBase;
class HTNValue;

class HTNConstant final : public HTNNodeBase
{
public:
    explicit HTNConstant(std::unique_ptr<const HTNValue> inName, const std::shared_ptr<const HTNValue>& inValue);
    ~HTNConstant();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                            GetName() const;
    const std::shared_ptr<const HTNValue>& GetValue() const;

private:
    std::unique_ptr<const HTNValue> mName;
    std::shared_ptr<const HTNValue> mValue;
};

inline const std::shared_ptr<const HTNValue>& HTNConstant::GetValue() const
{
    return mValue;
}
