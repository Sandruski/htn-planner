#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>

class HTNConditionBase;
class HTNValue;
class HTNDomain;

class HTNAxiom final : public HTNNodeBase
{
public:
    explicit HTNAxiom(std::unique_ptr<const HTNValue> inName);
    ~HTNAxiom();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                                    GetName() const;
    void                                           SetCondition(const std::shared_ptr<const HTNConditionBase>& inCondition);
    const std::shared_ptr<const HTNConditionBase>& GetCondition() const;
    void                                           SetParent(const std::weak_ptr<const HTNDomain>& inParent);
    const std::weak_ptr<const HTNDomain>&          GetParent() const;

private:
    std::unique_ptr<const HTNValue>         mName;
    std::shared_ptr<const HTNConditionBase> mCondition;
    std::weak_ptr<const HTNDomain>          mParent;
};

inline void HTNAxiom::SetCondition(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
    mCondition = inCondition;
}

inline const std::shared_ptr<const HTNConditionBase>& HTNAxiom::GetCondition() const
{
    return mCondition;
}

inline void HTNAxiom::SetParent(const std::weak_ptr<const HTNDomain>& inParent)
{
    mParent = inParent;
}

inline const std::weak_ptr<const HTNDomain>& HTNAxiom::GetParent() const
{
    return mParent;
}
