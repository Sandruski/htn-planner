#include "HTNPlannerHook.h"

#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Log.h"
#include "Runtime/HTNDecompositionContext.h"

bool HTNPlannerHook::ParseDomain(const std::string& inDomainPath)
{
    return mInterpreter.Init(inDomainPath);
}

std::vector<HTNTaskInstance> HTNPlannerHook::MakePlan(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const
{
    const std::vector<std::shared_ptr<const HTNTask>> Plan = mInterpreter.Interpret(inEntryPointName, ioDecompositionContext);
    if (Plan.empty())
    {
        LOG_ERROR("Interpret failed");
        return {};
    }

    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    std::vector<HTNTaskInstance> PlanInstance;
    PlanInstance.reserve(Plan.size());

    for (const std::shared_ptr<const HTNTask>& Task : Plan)
    {
        const std::string& Name = Task->GetName();

        std::vector<HTNAtom>                                InstanceArguments;
        const std::shared_ptr<const HTNMethod>&             Method    = Task->GetMethod();
        const std::vector<std::shared_ptr<const HTNValue>>& Arguments = Task->GetArguments();
        for (const std::shared_ptr<const HTNValue>& Argument : Arguments)
        {
            const HTNAtom& ArgumentValue = Argument->GetValue();
            if (ArgumentValue.IsType<std::string>())
            {
                const std::string& VariableName = ArgumentValue.GetValue<std::string>();
                const HTNAtom*     Variable     = CurrentDecomposition.FindVariable(Method, VariableName);
                if (Variable)
                {
                    InstanceArguments.emplace_back(*Variable);
                    continue;
                }
            }

            InstanceArguments.emplace_back(ArgumentValue);
        }

        PlanInstance.emplace_back(Name, InstanceArguments);
    }

    return PlanInstance;
}
