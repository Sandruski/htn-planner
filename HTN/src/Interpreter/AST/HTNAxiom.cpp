#include "Interpreter/AST/HTNAxiom.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNAxiom::HTNAxiom(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
    : mName(std::move(inName)), mArguments(inArguments)
{
}

HTNAxiom::~HTNAxiom() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNAxiom::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNAxiom::ToString() const
{
    std::string Name = GetName();

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Name.append(std::format(" {}", Argument->ToString()));
    }

    return Name;
}

std::string HTNAxiom::GetName() const
{
    return mName ? mName->ToString() : "Invalid Axiom";
}
