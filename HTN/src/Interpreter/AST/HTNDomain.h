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
    explicit HTNDomain(std::unique_ptr<const HTNValue> inName, const bool inIsTopLevel);
    ~HTNDomain();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::shared_ptr<const HTNAxiom>  FindAxiomByName(const std::string& inAxiomName) const;
    std::shared_ptr<const HTNMethod> FindMethodByName(const std::string& inMethodName) const;

    std::string                                             GetName() const;
    void                                                    SetAxioms(const std::vector<std::shared_ptr<const HTNAxiom>>& inAxioms);
    const std::vector<std::shared_ptr<const HTNAxiom>>&     GetAxioms() const;
    void                                                    SetConstants(const std::vector<std::shared_ptr<const HTNConstants>>& inConstants);
    const std::vector<std::shared_ptr<const HTNConstants>>& GetConstants() const;
    void                                                    SetMethods(const std::vector<std::shared_ptr<const HTNMethod>>& inMethods);
    const std::vector<std::shared_ptr<const HTNMethod>>&    GetMethods() const;
    bool                                                    IsTopLevel() const;

private:
    std::unique_ptr<const HTNValue>                  mName;
    std::vector<std::shared_ptr<const HTNAxiom>>     mAxioms;
    std::vector<std::shared_ptr<const HTNConstants>> mConstants;
    std::vector<std::shared_ptr<const HTNMethod>>    mMethods;
    bool                                             mIsTopLevel = false;
};

inline void HTNDomain::SetAxioms(const std::vector<std::shared_ptr<const HTNAxiom>>& inAxioms)
{
    mAxioms = inAxioms;
}

inline const std::vector<std::shared_ptr<const HTNAxiom>>& HTNDomain::GetAxioms() const
{
    return mAxioms;
}

inline void HTNDomain::SetConstants(const std::vector<std::shared_ptr<const HTNConstants>>& inConstants)
{
    mConstants = inConstants;
}

inline const std::vector<std::shared_ptr<const HTNConstants>>& HTNDomain::GetConstants() const
{
    return mConstants;
}

inline void HTNDomain::SetMethods(const std::vector<std::shared_ptr<const HTNMethod>>& inMethods)
{
    mMethods = inMethods;
}

inline const std::vector<std::shared_ptr<const HTNMethod>>& HTNDomain::GetMethods() const
{
    return mMethods;
}

inline bool HTNDomain::IsTopLevel() const
{
    return mIsTopLevel;
}
