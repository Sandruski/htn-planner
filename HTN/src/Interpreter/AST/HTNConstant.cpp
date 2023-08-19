#include "Interpreter/AST/HTNConstant.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNConstant::HTNConstant(std::unique_ptr<const HTNValue> inName, const std::shared_ptr<const HTNValue>& inValue)
    : mName(std::move(inName)), mValue(inValue)
{
}

HTNConstant::~HTNConstant() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNConstant::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNConstant::ToString() const
{
    return std::format("{} {}", GetName(), mValue->ToString());
}

std::string HTNConstant::GetName() const
{
    return mName->ToString();
}
