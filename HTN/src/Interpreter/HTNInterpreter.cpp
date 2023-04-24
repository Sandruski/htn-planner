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

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Interpret(const HTNWorldState& inWorldState) const
{
	if (!mDomain)
	{
		return {};
	}


	HTNDecompositionContext DecompositionContext = HTNDecompositionContext(inWorldState);

	return mDomain->Accept(*this);
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit(const HTNDomain& inDomain) const
{
	// Dummy root compound task
	const std::shared_ptr<const HTNTask> RootCompoundTask = std::make_shared<HTNTask>(EHTNTaskType::COMPOUND, std::make_unique<HTNValue>(HTNToken(HTNTokenType::IDENTIFIER, mRootCompoundTaskName, 0, 0, mRootCompoundTaskName)));

	std::vector<std::shared_ptr<const HTNTask>> TasksToProcess;
	TasksToProcess.emplace_back(RootCompoundTask);

	std::vector<std::shared_ptr<const HTNTask>> Plan;

	while (!TasksToProcess.empty())
	{
		const std::shared_ptr<const HTNTask> CurrentTask = TasksToProcess.back();
		TasksToProcess.pop_back();
		
		switch (CurrentTask->GetType())
		{
		case EHTNTaskType::PRIMITIVE:
		{
			Plan.emplace_back(CurrentTask);
			break;
		}
		case EHTNTaskType::COMPOUND:
		{
			std::shared_ptr<const HTNMethod> Method = inDomain.FindMethodByName(Method->GetName());
			if (!Method)
			{
				LOG_ERROR("Method {} not found", CurrentTask->GetName());
				// TODO salvarez
			}

			const std::vector<std::shared_ptr<const HTNBranch>>& Branches = Method->GetBranches();
			for (const std::shared_ptr<const HTNBranch>& Branch : Branches)
			{
				//const std::vector<std::shared_ptr<const HTNCondition>>& Conditions = Branch->GetConditions();
				// TODO salvarez Check conditions

				const std::vector<std::shared_ptr<const HTNTask>>& Tasks = Branch->GetTasks();
				for (const std::shared_ptr<const HTNTask>& Tasks : Tasks)
				{
					TasksToProcess.emplace_back(Tasks);
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

	return Plan;
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit([[maybe_unused]] const HTNMethod& inMethod) const
{
	return {};
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit([[maybe_unused]] const HTNBranch& inBranch) const
{
	return {};
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit([[maybe_unused]] const HTNCondition& inCondition) const
{
	return {};
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit([[maybe_unused]] const HTNTask& inTask) const
{
	return {};
}

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNInterpreter::Visit([[maybe_unused]] const HTNValue& inValue) const
{
	return {};
}
