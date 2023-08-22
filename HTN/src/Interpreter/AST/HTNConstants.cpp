#include "Interpreter/AST/HTNConstants.h"

#include "Interpreter/AST/HTNConstant.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNConstants::HTNConstants(const std::shared_ptr<const HTNValue>& inIDArgument, const std::vector<std::shared_ptr<const HTNConstant>>& inConstants)
    : mIDArgument(inIDArgument), mConstants(inConstants)
{
}

HTNConstants::~HTNConstants() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNConstants::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNConstants::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNConstants::ToString() const
{
    return GetID();
}
