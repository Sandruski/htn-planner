#include "Planner/HTNDatabaseHook.h"

#include "Helpers/HTNFileHandler.h"
#include "Helpers/HTNToken.h"

bool HTNDatabaseHook::ParseWorldStateFile(const std::string& inWorldStateFilePath)
{
    const HTNFileHandler WorldStateFileHandler = HTNFileHandler(inWorldStateFilePath);
    std::string          WorldStateText;
    if (!WorldStateFileHandler.ReadFile(WorldStateText))
    {
        LOG_ERROR("World state [{}] could not be read", inWorldStateFilePath);
        return false;
    }

    std::vector<HTNToken> Tokens;
    if (!mWorldStateLexer.Lex(WorldStateText, Tokens))
    {
        LOG_ERROR("World state [{}] could not be lexed", inWorldStateFilePath);
        return false;
    }

    HTNWorldState WorldState;
    if (!mWorldStateParser.Parse(Tokens, WorldState))
    {
        LOG_ERROR("World state [{}] could not be parsed", inWorldStateFilePath);
        return false;
    }

    mWorldState = WorldState;

    return true;
}
