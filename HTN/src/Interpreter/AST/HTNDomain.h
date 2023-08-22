#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNMethod;
class HTNValue;
class HTNAxiom;
class HTNConstants;

class HTNDomain final : public HTNNodeBase
{
public:
    explicit HTNDomain(const std::shared_ptr<const HTNValue>& inIDArgument, const std::vector<std::shared_ptr<const HTNConstants>>& inConstants,
                       const std::vector<std::shared_ptr<const HTNAxiom>>& inAxioms, const std::vector<std::shared_ptr<const HTNMethod>>& inMethods,
                       const bool inIsTopLevel);
    ~HTNDomain();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 GetID() const final;
    std::string                                 ToString() const final;

    std::shared_ptr<const HTNAxiom>  FindAxiomByID(const std::string& inID) const;
    std::shared_ptr<const HTNMethod> FindMethodByID(const std::string& inID) const;

    const std::shared_ptr<const HTNValue>&                  GetIDArgument() const;
    const std::vector<std::shared_ptr<const HTNConstants>>& GetConstants() const;
    const std::vector<std::shared_ptr<const HTNAxiom>>&     GetAxioms() const;
    const std::vector<std::shared_ptr<const HTNMethod>>&    GetMethods() const;
    bool                                                    IsTopLevel() const;

private:
    // ID of the domain, which is unique
    std::shared_ptr<const HTNValue> mIDArgument;

    // Constants of the domain
    std::vector<std::shared_ptr<const HTNConstants>> mConstants;

    // Axioms of the domain
    std::vector<std::shared_ptr<const HTNAxiom>> mAxioms;

    // Methods of the domain
    std::vector<std::shared_ptr<const HTNMethod>> mMethods;

    // Whether the domain serves as an entry point for a decomposition
    bool mIsTopLevel = false;
};

inline const std::shared_ptr<const HTNValue>& HTNDomain::GetIDArgument() const
{
    return mIDArgument;
}

inline const std::vector<std::shared_ptr<const HTNConstants>>& HTNDomain::GetConstants() const
{
    return mConstants;
}

inline const std::vector<std::shared_ptr<const HTNAxiom>>& HTNDomain::GetAxioms() const
{
    return mAxioms;
}

inline const std::vector<std::shared_ptr<const HTNMethod>>& HTNDomain::GetMethods() const
{
    return mMethods;
}

inline bool HTNDomain::IsTopLevel() const
{
    return mIsTopLevel;
}
