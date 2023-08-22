#include "Interpreter/HTNInterpreter.h"

#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Interpreter/HTNLexer.h"
#include "Interpreter/HTNParser.h"
#include "Interpreter/HTNToken.h"
#include "Runtime/HTNDecompositionContext.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>

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

HTNInterpreter::~HTNInterpreter() = default;

bool HTNInterpreter::Init(const std::string& inDomainPath)
{
    mDomain = nullptr;

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
    mDomain          = Parser.Parse();
    if (!mDomain)
    {
        LOG_ERROR("Parse failed");
        return false;
    }

    return true;
}

std::vector<std::shared_ptr<const HTNTask>> HTNInterpreter::Interpret(const std::string&       inEntryPointName,
                                                                      HTNDecompositionContext& ioDecompositionContext) const
{
    if (!mDomain)
    {
        LOG_ERROR("Domain is null");
        return {};
    }

    const HTNDecompositionScope DomainScope          = HTNDecompositionScope(ioDecompositionContext, mDomain);
    HTNDecompositionRecord&     CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root compound task
    std::vector<std::shared_ptr<const HTNValue>> RootTaskArguments = {
        std::make_shared<const HTNValue>(HTNAtom(inEntryPointName), HTNValueType::LITERAL)};
    const std::shared_ptr<const HTNTask> RootTask = std::make_shared<HTNTask>(RootTaskArguments, HTNTaskType::COMPOUND);
    CurrentDecomposition.PushTaskToProcess(RootTask);

    while (CurrentDecomposition.HasTasksToProcess())
    {
        const std::shared_ptr<const HTNTask> CurrentTask = CurrentDecomposition.PopTaskToProcess();
        if (!CurrentTask)
        {
            break;
        }

        CurrentDecomposition.SetCurrentTask(CurrentTask);

        switch (CurrentTask->GetType())
        {
        case HTNTaskType::PRIMITIVE: {
            CurrentDecomposition.AddTaskToPlan(CurrentTask);
            break;
        }
        case HTNTaskType::COMPOUND: {
            const std::string                      CurrentTaskID = CurrentTask->GetID();
            const std::shared_ptr<const HTNMethod> CurrentMethod = mDomain->FindMethodByID(CurrentTaskID);
            if (!CurrentMethod)
            {
                LOG_ERROR("Method [{}] not found", CurrentTaskID);
                break;
            }

            const HTNDecompositionScope CurrentMethodScope = HTNDecompositionScope(ioDecompositionContext, CurrentMethod);
            const std::string           CurrentScopeID     = ioDecompositionContext.MakeCurrentScopeID();

            // TODO salvarez Fix
            /*
            if (ioDecompositionContext.GetCurrentMethod() != CurrentMethod)
            {
                // Initialize the input arguments of the method with the arguments of the compound task
                const std::shared_ptr<const HTNNodeBase>            Scope           = CurrentTask->GetScope();
                const std::vector<std::shared_ptr<const HTNValue>>& TaskArguments   = CurrentTask->GetArguments();
                const std::vector<std::shared_ptr<const HTNValue>>& MethodArguments = CurrentMethod->GetArguments();
                if (!HTN::Helpers::CopyArguments(CurrentDecomposition, TaskArguments, MethodArguments, Scope, CurrentMethod, {},
                                                 HTN::Helpers::InputPrefixes))
                {
                    LOG_ERROR("Arguments could not be copied from method to method [{}]", CurrentMethod->GetName());
                    break;
                }
            }
            */

            const unsigned int                                   CurrentBranchIndex = CurrentDecomposition.GetOrAddIndex(CurrentScopeID);
            const std::vector<std::shared_ptr<const HTNBranch>>& CurrentBranches    = CurrentMethod->GetBranches();
            if (CurrentBranchIndex >= CurrentBranches.size())
            {
                break;
            }

            const std::shared_ptr<const HTNBranch>&        CurrentBranch       = CurrentBranches[CurrentBranchIndex];
            const std::shared_ptr<const HTNConditionBase>& CurrentPreCondition = CurrentBranch->GetPreCondition();
            bool                                           HasBoundVariables   = false;
            const bool Result = CurrentPreCondition ? CurrentPreCondition->Check(ioDecompositionContext, HasBoundVariables) : true;
            if (Result)
            {
                const std::vector<std::shared_ptr<const HTNTask>>& Tasks = CurrentBranch->GetTasks();
                for (int i = static_cast<int>(Tasks.size()) - 1; i >= 0; --i)
                {
                    const std::shared_ptr<const HTNTask>& Task = Tasks[i];
                    CurrentDecomposition.PushTaskToProcess(Task);
                }
            }
            else
            {
                if (CurrentBranchIndex < CurrentBranches.size() - 1)
                {
                    CurrentDecomposition.PushTaskToProcess(CurrentTask);

                    // Continue
                    CurrentDecomposition.IncrementIndex(CurrentScopeID);
                }
                else // Last branch
                {
                    // Restore state: unbound variables but updated indices
                    ioDecompositionContext.RestoreDecomposition();

                    CurrentDecomposition.PushTaskToProcess(CurrentDecomposition.GetCurrentTask());
                }
            }
            break;
        }
        default: {
            assert(false);
        }
        }
    }

    return CurrentDecomposition.GetPlan();
}
