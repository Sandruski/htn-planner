#include "Planner/HTNPlannerHook.h"

#include "Domain/AST/HTNDomainNode.h"
#include "Domain/Parser/HTNDomainLexer.h"
#include "Domain/Parser/HTNDomainParser.h"
#include "HTNFileHandler.h"
#include "HTNLog.h"
#include "HTNToken.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNTaskResult.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    const HTNFileHandler DomainFileHandler = HTNFileHandler(inDomainFilePath);
    std::string          DomainFileText;
    if (!DomainFileHandler.ReadFile(DomainFileText))
    {
        LOG_ERROR("Domain [{}] could not be read", inDomainFilePath);
        return false;
    }

    HTNDomainLexer        DomainLexer = HTNDomainLexer(DomainFileText);
    std::vector<HTNToken> Tokens;
    if (!DomainLexer.Lex(Tokens))
    {
        LOG_ERROR("Domain [{}] could not be lexed", inDomainFilePath);
        return false;
    }

    HTNDomainParser                            DomainParser = HTNDomainParser(Tokens);
    const std::shared_ptr<const HTNDomainNode> DomainNode   = DomainParser.Parse();
    if (!DomainNode)
    {
        LOG_ERROR("Domain [{}] could not be parsed", inDomainFilePath);
        return false;
    }

    mDomainNode = DomainNode;

    return true;
}

std::vector<HTNTaskResult> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const
{
    if (!mDomainNode)
    {
        LOG_ERROR("Domain node is null");
        return {};
    }

    HTNInterpreter             Interpreter = HTNInterpreter(mDomainNode, inEntryPointName, inWorldState);
    std::vector<HTNTaskResult> Plan;
    if (!Interpreter.Interpret(Plan))
    {
        LOG_ERROR("Domain [{}] could not be interpreted", mDomainNode->GetID());
        return {};
    }

    return Plan;
}
