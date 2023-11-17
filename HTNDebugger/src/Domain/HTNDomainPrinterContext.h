#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include <memory>

class HTNDomainNode;

class HTNDomainPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDomainPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNDomainPrinterContext::GetDomainNode() const
{
    return mDomainNode;
}
#endif