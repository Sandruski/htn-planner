#include "Interpreter/AST/HTNDomain.h"

#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNConstants.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

HTNDomain::HTNDomain(const std::shared_ptr<const HTNValue>& inIDArgument, const std::vector<std::shared_ptr<const HTNConstants>>& inConstants,
                     const std::vector<std::shared_ptr<const HTNAxiom>>& inAxioms, const std::vector<std::shared_ptr<const HTNMethod>>& inMethods,
                     const bool inIsTopLevel)
    : mIDArgument(inIDArgument), mConstants(inConstants), mAxioms(inAxioms), mMethods(inMethods), mIsTopLevel(inIsTopLevel)
{
}

HTNDomain::~HTNDomain() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNDomain::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNDomain::GetID() const
{
    return GetIDArgument()->ToString();
}

std::string HTNDomain::ToString() const
{
    std::string Description = GetID();

    if (mIsTopLevel)
    {
        Description.append(" top_level_domain");
    }

    return Description;
}

std::shared_ptr<const HTNAxiom> HTNDomain::FindAxiomByID(const std::string& inID) const
{
    for (const std::shared_ptr<const HTNAxiom>& Axiom : mAxioms)
    {
        if (inID == Axiom->GetID())
        {
            return Axiom;
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNMethod> HTNDomain::FindMethodByID(const std::string& inID) const
{
    for (const std::shared_ptr<const HTNMethod>& Method : mMethods)
    {
        if (inID == Method->GetID())
        {
            return Method;
        }
    }

    return nullptr;
}
