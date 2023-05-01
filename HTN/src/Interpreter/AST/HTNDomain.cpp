#include "Interpreter/AST/HTNDomain.h"

#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNDomain::HTNDomain(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNMethod>>& inMethods)
	: mName(std::move(inName)), mMethods(inMethods)
{
}

HTNDomain::~HTNDomain() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNDomain::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

std::string HTNDomain::ToString() const
{
	return GetName();
}

std::shared_ptr<const HTNMethod> HTNDomain::FindMethodByName(const std::string& inMethodName) const
{
	for (const std::shared_ptr<const HTNMethod>& Method : mMethods)
	{
		if (inMethodName == Method->GetName())
		{
			return Method;
		}
	}

	return nullptr;
}

std::string HTNDomain::GetName() const
{
	return mName ? mName->ToString() : "Invalid Domain";
}
