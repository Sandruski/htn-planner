// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNParserContextBase.h"

#include <memory>
#include <vector>

class HTNDomainNode;
class HTNToken;

/**
 * Contextual data of a parser of a domain
 */
class HTNDomainParserContext final : public HTNParserContextBase
{
public:
    explicit HTNDomainParserContext(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode);

    // Returns the domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

    // Returns the domain node
    HTN_NODISCARD std::shared_ptr<const HTNDomainNode>& GetDomainNodeMutable();

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    std::shared_ptr<const HTNDomainNode>& mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNDomainParserContext::GetDomainNode() const
{
    return mDomainNode;
}

inline std::shared_ptr<const HTNDomainNode>& HTNDomainParserContext::GetDomainNodeMutable()
{
    return mDomainNode;
}
