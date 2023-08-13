#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNBranch;
class HTNDecompositionContext;
class HTNValue;

class HTNMethod final : public HTNNodeBase
{
public:
    explicit HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments,
                       const std::vector<std::shared_ptr<const HTNBranch>>& inBranches);
    explicit HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments);
    ~HTNMethod();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::string                                          GetName() const;
    const std::vector<std::shared_ptr<const HTNValue>>&  GetArguments() const;
    void                                                 SetBranches(const std::vector<std::shared_ptr<const HTNBranch>>& inBranches);
    const std::vector<std::shared_ptr<const HTNBranch>>& GetBranches() const;

private:
    std::unique_ptr<const HTNValue>               mName;
    std::vector<std::shared_ptr<const HTNValue>>  mArguments;
    std::vector<std::shared_ptr<const HTNBranch>> mBranches;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNMethod::GetArguments() const
{
    return mArguments;
}

inline void HTNMethod::SetBranches(const std::vector<std::shared_ptr<const HTNBranch>>& inBranches)
{
    mBranches = inBranches;
}

inline const std::vector<std::shared_ptr<const HTNBranch>>& HTNMethod::GetBranches() const
{
    return mBranches;
}
