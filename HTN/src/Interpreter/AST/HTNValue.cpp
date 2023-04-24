#include "Interpreter/AST/HTNValue.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNValue::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

std::string HTNValue::ToString() const
{
	return GetName();
}

std::string HTNValue::GetName() const
{
	return mValue.GetLexeme();
}
