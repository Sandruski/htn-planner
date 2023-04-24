#include "Interpreter/HTNPrinter.h"

/*
#include "Interpreter/AST/HTNDomain.h"
#include "Log.h"

HTNPrinter::HTNPrinter(std::unique_ptr<const HTNDomain> inDomain)
	: mDomain(std::move(inDomain))
{
}

HTNPrinter::~HTNPrinter() = default;

std::string HTNPrinter::Print() const
{
	if (!mDomain)
	{
		return "";
	}

	std::any Value;
	const bool Result = mDomain->Accept(*this, Value);
	if (!Result)
	{
		return false;
	}

	return std::any_cast<std::string>(Value);
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNDomain& inDomain, [[maybe_unused]] std::any& outValue) const
{
	return true;
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNMethod& inMethod, [[maybe_unused]] std::any& outValue) const
{
	return true;
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNBranch& inBranch, [[maybe_unused]] std::any& outValue) const
{
	return true;
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNPrecondition& inPrecondition, [[maybe_unused]] std::any& outValue) const
{
	return true;
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNPrimitiveTask& inTask, [[maybe_unused]] std::any& outValue) const
{
	return true;
}

bool HTNPrinter::Visit([[maybe_unused]] const HTNValue& inValue, [[maybe_unused]] std::any& outValue) const
{
	return true;
}
*/