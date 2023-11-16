#pragma once

#include "Parser/HTNParserContextBase.h"
#include "WorldState/HTNWorldState.h"

#include <vector>

class HTNToken;

class HTNWorldStateParserContext : public HTNParserContextBase
{
public:
    explicit HTNWorldStateParserContext(const std::vector<HTNToken>& inTokens);

    const HTNWorldState& GetWorldState() const;
    HTNWorldState& GetWorldStateMutable();

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNWorldState mWorldState;
};

inline const HTNWorldState& HTNWorldStateParserContext::GetWorldState() const
{
    return mWorldState;
}

inline HTNWorldState& HTNWorldStateParserContext::GetWorldStateMutable()
{
    return mWorldState;
}
