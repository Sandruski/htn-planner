#pragma once

#include "Interpreter/AST/HTNNodeBase.h"

#include <memory>
#include <vector>

class HTNMethod;
class HTNValue;

class HTNDomain final : public HTNNodeBase
{
public:
    explicit HTNDomain(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNMethod>>& inMethods,
                       const bool inIsTopLevel);
    ~HTNDomain();

    std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
    std::string                                 ToString() const final;

    std::shared_ptr<const HTNMethod> FindMethodByName(const std::string& inMethodName) const;

    std::string                                          GetName() const;
    const std::vector<std::shared_ptr<const HTNMethod>>& GetMethods() const;
    bool                                                 IsTopLevel() const;

private:
    std::unique_ptr<const HTNValue>               mName;
    std::vector<std::shared_ptr<const HTNMethod>> mMethods;
    bool                                          mIsTopLevel = false;
};

inline const std::vector<std::shared_ptr<const HTNMethod>>& HTNDomain::GetMethods() const
{
    return mMethods;
}

inline bool HTNDomain::IsTopLevel() const
{
    return mIsTopLevel;
}
