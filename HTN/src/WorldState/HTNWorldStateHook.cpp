#include "WorldState/HTNWorldStateHook.h"

#include "HTNFileHandler.h"
#include "HTNLog.h"
#include "HTNToken.h"
#include "WorldState/Parser/HTNWorldStateParser.h"
#include "WorldState/Parser/HTNWorldStateLexer.h"

bool HTNWorldStateHook::ImportWorldStateFile(const std::string& inWorldStateFilePath)
{
    const HTNFileHandler WorldStateFileHandler = HTNFileHandler(inWorldStateFilePath);
    std::string          WorldStateFileText;
    if (!WorldStateFileHandler.ReadFile(WorldStateFileText))
    {
        LOG_ERROR("World state [{}] could not be read", inWorldStateFilePath);
        return false;
    }

    HTNWorldStateLexer    WorldStateLexer = HTNWorldStateLexer(WorldStateFileText);
    std::vector<HTNToken> Tokens;
    if (!WorldStateLexer.Lex(Tokens))
    {
        LOG_ERROR("World state [{}] could not be lexed", inWorldStateFilePath);
        return false;
    }

    HTNWorldStateParser WorldStateParser = HTNWorldStateParser(Tokens);
    HTNWorldState       WorldState;
    if (!WorldStateParser.Parse(WorldState))
    {
        LOG_ERROR("World state [{}] could not be parsed", inWorldStateFilePath);
        return false;
    }

    mWorldState = WorldState;

    return true;
}
