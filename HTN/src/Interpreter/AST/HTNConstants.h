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
    explicit HTNConstants(const std::shared_ptr<const HTNValue>& inIDArgument, const std::vector<std::shared_ptr<const HTNConstant>>& inConstants);
    ~HTNConstants();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::shared_ptr<const HTNValue>&                 GetIDArgument() const;
    const std::vector<std::shared_ptr<const HTNConstant>>& GetConstants() const;

private:
    // Name of the group of constants
    // - Unique within its domain
    std::shared_ptr<const HTNValue> mIDArgument;

    // Group of constants
    std::vector<std::shared_ptr<const HTNConstant>> mConstants;
};

inline const std::shared_ptr<const HTNValue>& HTNConstants::GetIDArgument() const
{
    return mIDArgument;
}

inline const std::vector<std::shared_ptr<const HTNConstant>>& HTNConstants::GetConstants() const
{
    return mConstants;
}
