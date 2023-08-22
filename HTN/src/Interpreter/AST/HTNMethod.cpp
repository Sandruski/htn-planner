#include "Interpreter/AST/HTNMethod.h"

#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNDecompositionContext.h"

#include <format>

HTNMethod::HTNMethod(const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const std::vector<std::shared_ptr<const HTNBranch>>& inBranches,
                     const bool inIsTopLevel)
    : mArguments(inArguments), mBranches(inBranches), mIsTopLevel(inIsTopLevel)
{
}

HTNMethod::~HTNMethod() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNMethod::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNMethod::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNMethod::ToString() const
{
    std::string Description;

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Description.append(std::format("{} ", Argument->ToString()));
    }

    if (mIsTopLevel)
    {
        Description.append("top_level_method");
    }

    return Description;
}
