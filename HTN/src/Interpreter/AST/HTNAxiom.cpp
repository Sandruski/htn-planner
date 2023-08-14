#include "Interpreter/AST/HTNAxiom.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNAxiom::HTNAxiom(std::unique_ptr<const HTNValue> inName) : mName(std::move(inName))
{
}

HTNAxiom::~HTNAxiom() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNAxiom::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNAxiom::ToString() const
{
    return GetName();
}

std::string HTNAxiom::GetName() const
{
    return mName ? mName->ToString() : "Invalid Axiom";
}
