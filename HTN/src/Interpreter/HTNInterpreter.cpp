#include "Interpreter/HTNInterpreter.h"

#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Interpreter/HTNLexer.h"
#include "Interpreter/HTNParser.h"
#include "Interpreter/HTNToken.h"
#include "Log.h"
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
        LOG_ERROR("File {} could not be opened", inPath);
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
    std::string Text;
    if (!ReadFile(inDomainPath, Text))
    {
        LOG_ERROR("File {} could not be read", inDomainPath);
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

    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    // Dummy root compound task
    const std::shared_ptr<const HTNTask> RootCompoundTask =
        std::make_shared<HTNTask>(HTNTaskType::COMPOUND, std::make_unique<HTNValue>(HTNAtom(inEntryPointName)), nullptr);
    CurrentDecomposition.PushTaskToProcess(RootCompoundTask);

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
            std::shared_ptr<const HTNMethod> CurrentMethod = mDomain->FindMethodByName(CurrentTask->GetName());
            if (!CurrentMethod)
            {
                LOG_ERROR("Method {} not found", CurrentTask->GetName());
                break;
            }

            // Initialize new method
            if (ioDecompositionContext.GetCurrentMethod() != CurrentMethod)
            {
                ioDecompositionContext.SetCurrentMethod(CurrentMethod);

                // Copy the values of the arguments of the old method to the new method
                const std::vector<std::shared_ptr<const HTNValue>>& PreviousArguments    = CurrentTask->GetArguments();
                const std::vector<std::shared_ptr<const HTNValue>>& CurrentArguments     = CurrentMethod->GetArguments();
                const size_t                                        PreviousArgumentSize = PreviousArguments.size();
                const size_t                                        CurrentArgumentSize  = CurrentArguments.size();
                if (PreviousArgumentSize != CurrentArgumentSize)
                {
                    LOG_ERROR("Method {} has {} arguments but is being called with {}", CurrentMethod->GetName(), CurrentArgumentSize,
                              PreviousArgumentSize);
                    break;
                }

                if (CurrentArgumentSize > 0)
                {
                    const std::shared_ptr<const HTNMethod>& PreviousMethod = CurrentTask->GetMethod();

                    for (size_t i = 0; i < CurrentArgumentSize; ++i)
                    {
                        const std::shared_ptr<const HTNValue>& PreviousArgument      = PreviousArguments[i];
                        const HTNAtom&                         PreviousArgumentValue = PreviousArgument->GetValue();
                        const std::string&                     PreviousVariableName  = PreviousArgumentValue.GetValue<std::string>();
                        const HTNAtom* PreviousVariable = CurrentDecomposition.FindVariable(PreviousMethod, PreviousVariableName);
                        if (!PreviousVariable)
                        {
                            LOG_ERROR("Variable {} not found in method {}", PreviousVariableName, PreviousMethod->GetName());
                            break;
                        }

                        const std::shared_ptr<const HTNValue>& CurrentArgument      = CurrentArguments[i];
                        const HTNAtom&                         CurrentArgumentValue = CurrentArgument->GetValue();
                        const std::string&                     CurrentVariableName  = CurrentArgumentValue.GetValue<std::string>();
                        HTNAtom& CurrentVariable = CurrentDecomposition.GetOrAddVariable(CurrentMethod, CurrentVariableName);
                        CurrentVariable          = *PreviousVariable;
                    }
                }
            }

            const unsigned int                                   CurrentBranchIndex = CurrentDecomposition.GetOrAddCurrentBranchIndex(CurrentMethod);
            const std::vector<std::shared_ptr<const HTNBranch>>& CurrentBranches    = CurrentMethod->GetBranches();
            if (CurrentBranchIndex >= CurrentBranches.size())
            {
                break;
            }

            const std::shared_ptr<const HTNBranch>&        CurrentBranch       = CurrentBranches[CurrentBranchIndex];
            const std::shared_ptr<const HTNConditionBase>& CurrentPreCondition = CurrentBranch->GetPreCondition();
            const bool                                     Result =
                CurrentPreCondition ? CurrentPreCondition->Check(ioDecompositionContext) : true; // TODO salvarez Time-slice condition result
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
                    CurrentDecomposition.IncrementCurrentBranchIndex(CurrentMethod);
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
