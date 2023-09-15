#include "Planner/HTNPlannerHook.h"

#include "HTNLog.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"
#include "Parser/HTNLexer.h"
#include "Parser/HTNParser.h"
#include "Planner/HTNTaskResult.h"

#include <fstream>
#include <sstream>

namespace
{
bool ReadFile(const std::string& inPath, std::string& outText)
{
    // Open file
    std::ifstream File;
    File.open(inPath);
    if (!File.is_open())
    {
        LOG_ERROR("File [{}] could not be opened", inPath);
        return false;
    }

    // Read file
    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    outText = Buffer.str();

    // Close file
    File.close();

    return true;
}
} // namespace

HTNPlannerHook::~HTNPlannerHook() = default;

bool HTNPlannerHook::ParseDomain(const std::string& inDomainPath)
{
    std::string Text;
    if (!ReadFile(inDomainPath, Text))
    {
        LOG_ERROR("File [{}] could not be read", inDomainPath);
        return false;
    }

    // Lex
    HTNLexer              Lexer = HTNLexer(Text);
    std::vector<HTNToken> Tokens;
    if (!Lexer.Lex(Tokens))
    {
        LOG_ERROR("Lex failed");
        return false;
    }

    // Parse
    HTNParser Parser = HTNParser(Tokens);
    mDomainNode      = Parser.Parse();
    if (!mDomainNode)
    {
        LOG_ERROR("Parse failed");
        return false;
    }

    return true;
}

std::vector<HTNTaskResult> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const
{
    // Interpret
    HTNInterpreter               Interpreter = HTNInterpreter(mDomainNode, inEntryPointName, inWorldState);
    std::vector<HTNTaskInstance> Plan;
    if (!Interpreter.Interpret(Plan))
    {
        LOG_ERROR("Interpret failed");
        return {};
    }

    // TODO salvarez Delete this
    std::vector<HTNTaskResult> PlanResult;
    PlanResult.reserve(Plan.size());

    /*
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (const HTNTaskScoped& TaskScoped : Plan)
    {
        const std::shared_ptr<const HTNTask>& Task = TaskScoped.GetTask();
        const std::string&                    Name = Task->GetIDArgument().;

        std::vector<HTNAtom>                                InstanceArguments;
        const std::vector<std::shared_ptr<const HTNValue>>& Arguments = Task->GetArguments();
        for (const std::shared_ptr<const HTNValue>& Argument : Arguments)
        {
            // TODO salvarez
            const HTNAtom& ArgumentValue = Argument->GetValue();
            if (Argument->GetType() != HTNValueType::VARIABLE)
            {
                InstanceArguments.emplace_back(ArgumentValue);
                continue;
            }

            const std::string& VariableName = ArgumentValue.GetValue<std::string>();
            const HTNAtom*     Variable     = CurrentDecomposition.FindVariable(Scope, VariableName);
            if (!Variable)
            {
                LOG_ERROR("Variable [{}] not found", VariableName);
                continue;
            }

            InstanceArguments.emplace_back(*Variable);
        }

        PlanInstance.emplace_back(Name, InstanceArguments);
    }
    */

    return PlanResult;
}
