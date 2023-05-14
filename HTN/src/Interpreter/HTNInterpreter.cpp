#include "Interpreter/HTNInterpreter.h"

#include "Interpreter/AST/HTNBranch.h"
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
}

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
	HTNLexer Lexer = HTNLexer(Text);
	std::vector<HTNToken> Tokens;
	if (!Lexer.Lex(Tokens))
	{
		LOG_ERROR("Lex failed");
		return false;
	}

	// Parse
	HTNParser Parser = HTNParser(Tokens);
	mDomain = Parser.Parse();
	if (!mDomain)
	{
		LOG_ERROR("Parse failed");
		return false;
	}

	return true;
}

std::vector<std::shared_ptr<const HTNTask>> HTNInterpreter::Interpret(const std::string& inEntryPointName, const HTNWorldState& inWorldState) const
{
	if (!mDomain)
	{
		LOG_ERROR("Domain is null");
		return {};
	}

	HTNDecompositionContext DecompositionContext = HTNDecompositionContext(inWorldState);
	HTNDecompositionRecord& CurrentDecomposition = DecompositionContext.GetCurrentDecompositionMutable();

	// Dummy root compound task
	const std::shared_ptr<const HTNTask> RootCompoundTask = std::make_shared<HTNTask>(EHTNTaskType::COMPOUND, std::make_unique<HTNValue>(HTNAtom(inEntryPointName)));
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
