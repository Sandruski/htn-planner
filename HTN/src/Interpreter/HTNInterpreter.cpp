#include "Interpreter/HTNInterpreter.h"

#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"
#include "Interpreter/HTNToken.h"
#include "Log.h"
#include "Runtime/HTNDecompositionContext.h"

#include <cassert>
#include <vector>

HTNInterpreter::HTNInterpreter(const std::shared_ptr<const HTNDomain>& inDomain, const std::string& inRootCompoundTaskName)
	: mDomain(inDomain), mRootCompoundTaskName(inRootCompoundTaskName)
{
}

HTNInterpreter::~HTNInterpreter() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNInterpreter::Interpret(const HTNWorldState& inWorldState)
{
	if (!mDomain)
	{
		return {};
	}

	HTNDecompositionContext DecompositionContext = HTNDecompositionContext(inWorldState);
	HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

	// Dummy root compound task
	const std::shared_ptr<const HTNTask> RootCompoundTask = std::make_shared<HTNTask>(EHTNTaskType::COMPOUND, std::make_unique<HTNValue>(HTNAtom(mRootCompoundTaskName.c_str())));
	CurrentDecomposition.PushTaskToProcess(RootCompoundTask);

	while (CurrentDecomposition.HasTasksToProcess())
	{
		const std::shared_ptr<const HTNTask> CurrentTask = CurrentDecomposition.PopTaskToProcess();
		switch (CurrentTask->GetType())
		{
		case EHTNTaskType::PRIMITIVE:
		{
			CurrentDecomposition.AddTaskToPlan(CurrentTask);
			break;
		}
		case EHTNTaskType::COMPOUND:
		{
			std::shared_ptr<const HTNMethod> Method = mDomain->FindMethodByName(CurrentTask->GetName());
			if (!Method)
			{
				LOG_ERROR("Method {} not found", CurrentTask->GetName());
				break;
			}
			
			const std::vector<std::shared_ptr<const HTNBranch>>& Branches = Method->GetBranches();
			if (Branches.empty())
			{
				LOG_ERROR("Method does not have branches", CurrentTask->GetName());
				break;
			}

			const unsigned int NextBranchIndex = CurrentDecomposition.GetNextBranchIndex();
			if (NextBranchIndex >= Branches.size())
			{
				LOG_ERROR("Next branch index {} is out of range", NextBranchIndex);
				break;
			}

			const std::shared_ptr<const HTNBranch>& Branch = Branches[NextBranchIndex];
			if (!Branch)
			{
				LOG_ERROR("Branch is null");
				break;
			}

			if (!Branch->Check(DecompositionContext))
			{
				DecompositionContext.RestoreDecomposition();
				CurrentDecomposition.IncrementNextBranchIndex();
				CurrentDecomposition.PushTaskToProcess(CurrentTask);
				break;
			}

			DecompositionContext.RecordDecomposition();
			CurrentDecomposition.ResetNextBranchIndex();
			const std::vector<std::shared_ptr<const HTNTask>>& Tasks = Branch->GetTasks();
			for (const std::shared_ptr<const HTNTask>& Task : Tasks)
			{
				CurrentDecomposition.PushTaskToProcess(Task);
			}

			break;
		}
		default:
		{
			assert(false);
		}
		}
	}

	return CurrentDecomposition.GetPlan();
}
