#include "Interpreter/AST/HTNDomain.h"

#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNConstants.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNDomain::HTNDomain(std::unique_ptr<const HTNValue> inName, const bool inIsTopLevel) : mName(std::move(inName)), mIsTopLevel(inIsTopLevel)
{
}

HTNDomain::~HTNDomain() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNDomain::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNDomain::ToString() const
{
    std::string Name = GetName();

    if (mIsTopLevel)
    {
        Name.append(" top_level_domain");
    }

    return Name;
}

std::shared_ptr<const HTNAxiom> HTNDomain::FindAxiomByName(const std::string& inName) const
{
    for (const std::shared_ptr<const HTNAxiom>& Axiom : mAxioms)
    {
        if (inName == Axiom->GetName())
        {
            return Axiom;
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNMethod> HTNDomain::FindMethodByName(const std::string& inName) const
{
    for (const std::shared_ptr<const HTNMethod>& Method : mMethods)
    {
        if (inName == Method->GetName())
        {
            return Method;
        }
    }

    return nullptr;
}

std::string HTNDomain::GetName() const
{
    return mName->ToString();
}
