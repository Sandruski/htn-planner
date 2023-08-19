#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNConditionBase;
class HTNConstant;
class HTNValue;

class HTNConstants final : public HTNNodeBase
{
public:
    explicit HTNConstants(std::unique_ptr<const HTNValue> inName);
    ~HTNConstants();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                                            GetName() const;
    void                                                   SetConstants(const std::vector<std::shared_ptr<const HTNConstant>>& inConstants);
    const std::vector<std::shared_ptr<const HTNConstant>>& GetConstants() const;

private:
    std::unique_ptr<const HTNValue>                 mName;
    std::vector<std::shared_ptr<const HTNConstant>> mConstants;
};

inline void HTNConstants::SetConstants(const std::vector<std::shared_ptr<const HTNConstant>>& inConstants)
{
    mConstants = inConstants;
}

inline const std::vector<std::shared_ptr<const HTNConstant>>& HTNConstants::GetConstants() const
{
    return mConstants;
}
