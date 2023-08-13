#include "Interpreter/AST/HTNMethod.h"

#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNDecompositionContext.h"

#include <format>

HTNMethod::HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
    : mName(std::move(inName)), mArguments(inArguments)
{
}

HTNMethod::HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments,
                     const std::vector<std::shared_ptr<const HTNBranch>>& inBranches)
    : mName(std::move(inName)), mArguments(inArguments), mBranches(inBranches)
{
}

HTNMethod::~HTNMethod() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNMethod::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNMethod::ToString() const
{
    std::string Name = GetName();

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Name.append(std::format(" {}", Argument->ToString()));
    }

    return Name;
}

std::string HTNMethod::GetName() const
{
    return mName ? mName->ToString() : "Invalid Method";
}
