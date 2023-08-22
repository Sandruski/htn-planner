#include "HTNPlannerHook.h"

#include "HTNLog.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
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

    // HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    std::vector<HTNTaskInstance> PlanInstance;
    PlanInstance.reserve(Plan.size());

    // TODO salvarez Do this on the interpreter side to be able to access the scope
    /*
    for (const std::shared_ptr<const HTNTask>& Task : Plan)
    {
        const std::string& Name = Task->GetName();

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
