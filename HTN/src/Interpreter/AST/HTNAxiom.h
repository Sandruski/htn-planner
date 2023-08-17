#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNValue;

class HTNAxiom final : public HTNNodeBase
{
public:
    explicit HTNAxiom(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments);
    ~HTNAxiom();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                                         GetName() const;
    const std::vector<std::shared_ptr<const HTNValue>>& GetArguments() const;
    void                                                SetCondition(const std::shared_ptr<const HTNConditionBase>& inCondition);
    const std::shared_ptr<const HTNConditionBase>&      GetCondition() const;

private:
    std::unique_ptr<const HTNValue>              mName;
    std::vector<std::shared_ptr<const HTNValue>> mArguments;
    std::shared_ptr<const HTNConditionBase>      mCondition;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNAxiom::GetArguments() const
{
    return mArguments;
}

inline void HTNAxiom::SetCondition(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
    mCondition = inCondition;
}

inline const std::shared_ptr<const HTNConditionBase>& HTNAxiom::GetCondition() const
{
    return mCondition;
}
