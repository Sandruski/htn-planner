#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNNodeVisitorBase;
class HTNTask;

class HTNNodeBase
{
public:
    virtual ~HTNNodeBase() = default;

    virtual std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const = 0;
    virtual std::string                                 ToString() const                                  = 0;

    void                                     SetScope(const std::shared_ptr<const HTNNodeBase>& inScope);
    const std::shared_ptr<const HTNNodeBase> GetScope() const;

private:
    std::weak_ptr<const HTNNodeBase> mScope;
};

inline void HTNNodeBase::SetScope(const std::shared_ptr<const HTNNodeBase>& inScope)
{
    mScope = inScope;
}

inline const std::shared_ptr<const HTNNodeBase> HTNNodeBase::GetScope() const
{
    return mScope.lock();
}
