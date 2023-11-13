#include "Planner/HTNPlannerHook.h"

#include "Domain/Nodes/HTNDomainNode.h"
#include "Helpers/HTNFileHandler.h"
#include "Helpers/HTNToken.h"

bool HTNPlannerHook::ParseDomainFile(const std::string& inDomainFilePath)
{
    const HTNFileHandler DomainFileHandler = HTNFileHandler(inDomainFilePath);
    std::string          DomainFileText;
    if (!DomainFileHandler.ReadFile(DomainFileText))
    {
        LOG_ERROR("Domain [{}] could not be read", inDomainFilePath);
        return false;
    }

    std::vector<HTNToken> Tokens;
    if (!mDomainLexer.Lex(DomainFileText, Tokens))
    {
        LOG_ERROR("Domain [{}] could not be lexed", inDomainFilePath);
        return false;
    }

    std::shared_ptr<const HTNDomainNode> DomainNode;
    if (!mDomainParser.Parse(Tokens, DomainNode))
    {
        LOG_ERROR("Domain [{}] could not be parsed", inDomainFilePath);
        return false;
    }

    mDomainNode = DomainNode;

    return true;
}

bool HTNPlannerHook::MakePlan(const std::string& inEntryPointID, HTNDecompositionContext& ioDecompositionContext)
{
    if (!mDomainNode)
    {
        LOG_ERROR("Domain node is null");
        return false;
    }

    if (!mDomainInterpreter.Interpret(mDomainNode, inEntryPointID, ioDecompositionContext))
    {
        LOG_ERROR("Domain [{}] could not be interpreted", mDomainNode->GetID());
        return false;
    }

    return true;
}
