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

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Interpret(const HTNWorldState& inWorldState)
{
	if (!mDomain)
	{
		return {};
	}


	HTNDecompositionContext DecompositionContext = HTNDecompositionContext(inWorldState);

	// Dummy root compound task
	const std::shared_ptr<const HTNTask> RootCompoundTask = std::make_shared<HTNTask>(EHTNTaskType::COMPOUND, std::make_unique<HTNValue>(HTNToken(HTNTokenType::IDENTIFIER, mRootCompoundTaskName, 0, 0, mRootCompoundTaskName)));
	mCurrentDecomposition.PushTaskToProcess(RootCompoundTask);

	while (mCurrentDecomposition.HasTasksToProcess())
	{
		const std::shared_ptr<const HTNTask> CurrentTask = mCurrentDecomposition.PopTaskToProcess();
		switch (CurrentTask->GetType())
		{
		case EHTNTaskType::PRIMITIVE:
		{
			mCurrentDecomposition.AddTaskToPlan(CurrentTask);
			break;
		}
		case EHTNTaskType::COMPOUND:
		{
			std::shared_ptr<const HTNMethod> Method = mDomain->FindMethodByName(CurrentTask->GetName());
			if (!Method)
			{
				LOG_ERROR("Method {} not found", CurrentTask->GetName());
				// TODO salvarez Rollback
			}

			unsigned int CurrentBranchIndex = 0;

			const std::vector<std::shared_ptr<const HTNBranch>>& Branches = Method->GetBranches();
			for (unsigned int i = 0; i < Branches.size(); ++i)
			{
				const std::shared_ptr<const HTNBranch>& Branch = Branches[i];
				if (!Branch)
				{
					LOG_ERROR("Branch is null");
					// TODO salvarez Rollback
				}

				if (!Branch->Check(DecompositionContext))
				{
					// TODO salvarez Rollback
				}

				RecordDecomposition();

				const std::vector<std::shared_ptr<const HTNTask>>& Tasks = Branch->GetTasks();
				for (const std::shared_ptr<const HTNTask>& Task : Tasks)
				{
					mCurrentDecomposition.PushTaskToProcess(Task);
				}
			}

			break;
		}
		default:
		{
			assert(false);
		}
		}
	}

	return mCurrentDecomposition.GetPlan();
}

void HTNInterpreter::RecordDecomposition()
{
	//mDecompositionHistory.emplace_back(mCurrentDecomposition.GetTasksToProcess(), const HTNPlan & inPlan, const unsigned int inNextBranchIndex)
}

void HTNInterpreter::RestoreDecomposition()
{

}
