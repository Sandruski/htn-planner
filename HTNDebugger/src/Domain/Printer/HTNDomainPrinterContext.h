// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNDomainNode;

class HTNDomainPrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDomainPrinterContext(const std::shared_ptr<const HTNDomainNode>& inDomainNode);

    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

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
