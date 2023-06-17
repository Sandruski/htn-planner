#include "Interpreter/AST/HTNMethod.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNDecompositionContext.h"

HTNMethod::HTNMethod(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const std::vector<std::shared_ptr<const HTNBranch>>& inBranches)
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
	return GetName();
}

std::string HTNMethod::GetName() const
{
	return mName ? mName->ToString() : "Invalid Method";
}
