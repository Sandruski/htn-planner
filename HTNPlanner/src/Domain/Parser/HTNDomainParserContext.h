#pragma once

#include "HTNPlannerMinimal.h"
#include "Parser/HTNParserContextBase.h"

#include <memory>
#include <vector>

class HTNDomainNode;
class HTNToken;

class HTNDomainParserContext : public HTNParserContextBase
{
public:
    explicit HTNDomainParserContext(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    std::shared_ptr<const HTNDomainNode>&       GetDomainNodeMutable();

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
