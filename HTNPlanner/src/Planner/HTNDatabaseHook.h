// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"
#include "WorldState/HTNWorldState.h"
#include "WorldState/Parser/HTNWorldStateLexer.h"
#include "WorldState/Parser/HTNWorldStateParser.h"

#include <string>

class HTNDatabaseHook
{
public:
    // Parses a world state file and initializes a world state
    bool ParseWorldStateFile(const std::string& inWorldStateFilePath);

    const HTNWorldState& GetWorldState() const;

private:
    HTNWorldState mWorldState;

    const HTNWorldStateLexer mWorldStateLexer = HTNWorldStateLexer();
    const HTNWorldStateParser mWorldStateParser = HTNWorldStateParser();
};

inline const HTNWorldState& HTNDatabaseHook::GetWorldState() const
{
    return mWorldState;
}
