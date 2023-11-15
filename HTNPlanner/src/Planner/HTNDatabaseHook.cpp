#include "Planner/HTNDatabaseHook.h"

#include "Helpers/HTNFileHandler.h"
#include "WorldState/Parser/HTNWorldStateLexerContext.h"
#include "WorldState/Parser/HTNWorldStateParserContext.h"

bool HTNDatabaseHook::ParseWorldStateFile(const std::string& inWorldStateFilePath)
{
    const HTNFileHandler WorldStateFileHandler = HTNFileHandler(inWorldStateFilePath);
    std::string          WorldStateText;
    if (!WorldStateFileHandler.ReadFile(WorldStateText))
    {
        LOG_ERROR("World state [{}] could not be read", inWorldStateFilePath);
        return false;
    }

    HTNWorldStateLexerContext WorldStateLexerContext = HTNWorldStateLexerContext(WorldStateText);
    if (!mWorldStateLexer.Lex(WorldStateLexerContext))
    {
        LOG_ERROR("World state [{}] could not be lexed", inWorldStateFilePath);
        return false;
    }

    const std::vector<HTNToken>& Tokens                  = WorldStateLexerContext.GetTokens();
    HTNWorldStateParserContext   WorldStateParserContext = HTNWorldStateParserContext(Tokens);
    if (!mWorldStateParser.Parse(WorldStateParserContext))
    {
        LOG_ERROR("World state [{}] could not be parsed", inWorldStateFilePath);
        return false;
    }

    mWorldState = WorldStateParserContext.GetWorldState();

    return true;
}
