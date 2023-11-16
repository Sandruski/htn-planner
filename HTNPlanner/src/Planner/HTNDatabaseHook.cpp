#include "Planner/HTNDatabaseHook.h"

#include "HTNFileHandler.h"
#include "Helpers/HTNToken.h"
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

    std::vector<HTNToken>     Tokens;
    HTNWorldStateLexerContext WorldStateLexerContext = HTNWorldStateLexerContext(WorldStateText, Tokens);
    if (!mWorldStateLexer.Lex(WorldStateLexerContext))
    {
        LOG_ERROR("World state [{}] could not be lexed", inWorldStateFilePath);
        return false;
    }

    mWorldState                                        = HTNWorldState();
    HTNWorldStateParserContext WorldStateParserContext = HTNWorldStateParserContext(Tokens, mWorldState);
    if (!mWorldStateParser.Parse(WorldStateParserContext))
    {
        LOG_ERROR("World state [{}] could not be parsed", inWorldStateFilePath);
        return false;
    }

    return true;
}
