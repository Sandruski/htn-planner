#pragma once

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

    const HTNWorldStateLexer  mWorldStateLexer;
    const HTNWorldStateParser mWorldStateParser;
};

inline const HTNWorldState& HTNDatabaseHook::GetWorldState() const
{
    return mWorldState;
}
