#include "Interpreter/AST/HTNAxiom.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNAxiom::HTNAxiom(const std::vector<std::shared_ptr<const HTNValue>>& inArguments, const std::shared_ptr<const HTNConditionBase>& inCondition)
    : mArguments(inArguments), mCondition(inCondition)
{
}

HTNAxiom::~HTNAxiom() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNAxiom::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNAxiom::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNAxiom::ToString() const
{
    std::string Description;

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Description.append(std::format("{} ", Argument->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}
