#include "Planner/HTNPlannerHook.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Parser/HTNDomainLexerContext.h"
#include "Domain/Parser/HTNDomainParserContext.h"
#include "Helpers/HTNFileHandler.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    const HTNFileHandler DomainFileHandler = HTNFileHandler(inDomainFilePath);
    std::string          DomainFileText;
    if (!DomainFileHandler.ReadFile(DomainFileText))
    {
        LOG_ERROR("Domain [{}] could not be read", inDomainFilePath);
        return false;
    }

    HTNDomainLexerContext DomainLexerContext = HTNDomainLexerContext(DomainFileText);
    if (!mDomainLexer.Lex(DomainLexerContext))
    {
        LOG_ERROR("Domain [{}] could not be lexed", inDomainFilePath);
        return false;
    }

    const std::vector<HTNToken>& Tokens              = DomainLexerContext.GetTokens();
    HTNDomainParserContext       DomainParserContext = HTNDomainParserContext(Tokens);
    if (!mDomainParser.Parse(DomainParserContext))
    {
        LOG_ERROR("Domain [{}] could not be parsed", inDomainFilePath);
        return false;
    }

    mDomainNode = DomainParserContext.GetDomainNode();

    return true;
}

bool HTNPlannerHook::MakePlan(const std::string& inEntryPointID, HTNDecompositionContext& ioDecompositionContext) const
{
    ioDecompositionContext.SetDomainNode(mDomainNode);
    ioDecompositionContext.SetEntryPointID(inEntryPointID);

    if (!mDomainInterpreter.Interpret(ioDecompositionContext))
    {
        LOG_ERROR("Domain [{}] could not be interpreted", mDomainNode->GetID());
        return false;
    }

    return true;
}
