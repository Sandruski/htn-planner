#include "Interpreter/AST/HTNConstants.h"

#include "Interpreter/AST/HTNConstant.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNConstants::HTNConstants(std::unique_ptr<const HTNValue> inName) : mName(std::move(inName))
{
}

HTNConstants::~HTNConstants() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNConstants::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNConstants::ToString() const
{
    return GetName();
}

std::string HTNConstants::GetName() const
{
    return mName ? mName->ToString() : "";
}
