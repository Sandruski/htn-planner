#include "Interpreter/AST/HTNConstant.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNConstant::HTNConstant(const std::vector<std::shared_ptr<const HTNValue>>& inArguments) : mArguments(inArguments)
{
}

HTNConstant::~HTNConstant() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNConstant::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNConstant::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNConstant::ToString() const
{
    return std::format("{} {}", GetID(), GetValueArgument()->ToString());
}
