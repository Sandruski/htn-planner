#pragma once

#include "HTNWorldState.h"

#include <string>

class HTNWorldStateHook
{
public:
    // Imports a world state file into the world state database
    bool ImportWorldStateFile(const std::string& inWorldStateFilePath);

    const HTNWorldState& GetWorldState() const;
    HTNWorldState& GetWorldStateMutable();

private:
    HTNWorldState mWorldState;
};

inline const HTNWorldState& HTNWorldStateHook::GetWorldState() const
{
    return mWorldState;
}

inline HTNWorldState& HTNWorldStateHook::GetWorldStateMutable()
{
    return mWorldState;
}
