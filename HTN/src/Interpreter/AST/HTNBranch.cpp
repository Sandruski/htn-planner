#include "Interpreter/AST/HTNBranch.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"

HTNBranch::HTNBranch(std::unique_ptr<const HTNValue> inName) : mName(std::move(inName))
{
}

HTNBranch::~HTNBranch() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNBranch::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNBranch::ToString() const
{
    return GetName();
}

std::string HTNBranch::GetName() const
{
    return mName ? mName->ToString() : "Invalid Branch";
}
