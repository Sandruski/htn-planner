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

    void                                     SetParent(const std::shared_ptr<const HTNNodeBase>& inParent);
    const std::shared_ptr<const HTNNodeBase> GetParent() const;

private:
    std::weak_ptr<const HTNNodeBase> mParent;
};

inline void HTNNodeBase::SetParent(const std::shared_ptr<const HTNNodeBase>& inParent)
{
    mParent = inParent;
}

inline const std::shared_ptr<const HTNNodeBase> HTNNodeBase::GetParent() const
{
    return mParent.lock();
}
