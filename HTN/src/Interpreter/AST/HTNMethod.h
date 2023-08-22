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
    explicit HTNMethod(const std::vector<std::shared_ptr<const HTNValue>>&  inArguments,
                       const std::vector<std::shared_ptr<const HTNBranch>>& inBranches, const bool inIsTopLevel);
    ~HTNMethod();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    const std::vector<std::shared_ptr<const HTNValue>>&  GetArguments() const;
    const std::shared_ptr<const HTNValue>&               GetIDArgument() const;
    const std::vector<std::shared_ptr<const HTNBranch>>& GetBranches() const;
    bool                                                 IsTopLevel() const;

private:
    // Arguments of the method
    // - 1: ID of the method, which is unique within its domain
    std::vector<std::shared_ptr<const HTNValue>> mArguments;

    // Branches of the method
    std::vector<std::shared_ptr<const HTNBranch>> mBranches;

    // Whether the method serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::vector<std::shared_ptr<const HTNValue>>& HTNMethod::GetArguments() const
{
    return mArguments;
}

inline const std::shared_ptr<const HTNValue>& HTNMethod::GetIDArgument() const
{
    return mArguments[0];
}

inline const std::vector<std::shared_ptr<const HTNBranch>>& HTNMethod::GetBranches() const
{
    return mBranches;
}

inline bool HTNMethod::IsTopLevel() const
{
    return mIsTopLevel;
}
