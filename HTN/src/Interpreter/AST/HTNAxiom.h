#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNValue;

class HTNAxiom final : public HTNNodeBase
{
public:
    explicit HTNAxiom(const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const std::shared_ptr<const HTNConditionBase>& inCondition);
    ~HTNAxiom();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::vector<std::shared_ptr<const HTNValue>>& GetArguments() const;
    const std::shared_ptr<const HTNValue>&              GetIDArgument() const;
    const std::shared_ptr<const HTNConditionBase>&      GetCondition() const;

private:
    // Arguments of the axiom
    // - 1: ID of the axiom, which is unique within its domain
    std::vector<std::shared_ptr<const HTNValue>> mArguments;

    // Condition of the axiom
    std::shared_ptr<const HTNConditionBase> mCondition;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNAxiom::GetArguments() const
{
    return mArguments;
}

inline const std::shared_ptr<const HTNValue>& HTNAxiom::GetIDArgument() const
{
    return mArguments[0];
}

inline const std::shared_ptr<const HTNConditionBase>& HTNAxiom::GetCondition() const
{
    return mCondition;
}
