// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNParserContextBase.h"

#include <vector>

class HTNToken;
class HTNWorldState;

class HTNWorldStateParserContext : public HTNParserContextBase
{
public:
    explicit HTNWorldStateParserContext(const std::vector<HTNToken>& inTokens, HTNWorldState& outWorldState);

    HTN_NODISCARD const HTNWorldState& GetWorldState() const;
    HTN_NODISCARD HTNWorldState& GetWorldStateMutable();

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
