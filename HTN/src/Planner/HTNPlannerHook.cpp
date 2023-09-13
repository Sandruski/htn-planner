#include "Planner/HTNPlannerHook.h"

#include "HTNLog.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Parser/AST/HTNValueNode.h"
#include "Planner/HTNTaskResult.h"

bool HTNPlannerHook::ParseDomain(const std::string& inDomainPath)
{
    return mInterpreter.Init(inDomainPath);
}

std::vector<HTNTaskResult> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const
{
    const std::vector<HTNTaskInstance> Plan = mInterpreter.Interpret(inEntryPointName, ioDecompositionContext);
    if (Plan.empty())
    {
        LOG_ERROR("Interpret failed");
        return {};
    }

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

    return PlanInstance;
}
