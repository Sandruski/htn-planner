#pragma once

#include "HTNPlannerMinimal.h"
#include "Parser/HTNParserContextBase.h"

#include <vector>

class HTNToken;
class HTNWorldState;

class HTNWorldStateParserContext : public HTNParserContextBase
{
public:
    explicit HTNWorldStateParserContext(const std::vector<HTNToken>& inTokens, HTNWorldState& outWorldState);

    const HTNWorldState& GetWorldState() const;
    HTNWorldState& GetWorldStateMutable();

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    HTNWorldState& mWorldState;
};

inline const HTNWorldState& HTNWorldStateParserContext::GetWorldState() const
{
    return mWorldState;
}

inline HTNWorldState& HTNWorldStateParserContext::GetWorldStateMutable()
{
    return mWorldState;
}
