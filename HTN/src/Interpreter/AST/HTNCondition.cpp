#include "Interpreter/AST/HTNCondition.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNDecompositionContext.h"

#include <cassert>

bool HTNConditionWorldStateQuery::Check(const HTNWorldState& inWorldState, const int inIndex) const
{
	return inWorldState.CheckIndex(mKey.c_str(), inIndex, mArguments);
}

std::vector<std::shared_ptr<const HTNTask>> HTNConditionBase::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

HTNCondition::HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
	: mName(std::move(inName)), mArguments(inArguments)
{
}

std::string HTNCondition::ToString() const
{
	return GetName();
}

bool HTNCondition::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
	const std::shared_ptr<const HTNCondition> This = shared_from_this();
	const HTNWorldState* WorldState = ioDecompositionContext.GetWorldState();

	const HTNAtom& NameValue = mName->GetValue();
	const std::string& Key = NameValue.GetValue<std::string>();

	// Add variables for the new arguments
	std::vector<HTNAtom*> Arguments;
	for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
	{
		const HTNAtom& ArgumentValue = Argument->GetValue();
		const std::string& VariableName = ArgumentValue.GetValue<std::string>();
		HTNAtom& Variable = CurrentDecomposition.GetOrAddVariable(VariableName);
		Arguments.emplace_back(&Variable);
	}

	// Try to bind the new variables
	const HTNConditionWorldStateQuery Condition = HTNConditionWorldStateQuery(Key, Arguments);
	const unsigned int ArgumentsCount = static_cast<unsigned int>(Arguments.size());
	const unsigned int FactEntriesCount = WorldState->GetNumFactEntries(Key, ArgumentsCount);
	for (unsigned int CurrentFactEntryIndex = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This); CurrentFactEntryIndex < FactEntriesCount; CurrentFactEntryIndex = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This))
	{
		const std::unordered_map<std::string, HTNAtom> OldVariables = CurrentDecomposition.GetVariables();
		if (Condition.Check(*WorldState, CurrentFactEntryIndex))
		{
			// Record state: unbound variables but updated indices
			HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
			NewDecomposition.SetVariables(OldVariables);
			ioDecompositionContext.RecordDecomposition(NewDecomposition);

			return true;
		}
	}

	return false;
}

std::string HTNCondition::GetName() const
{
	return mName ? mName->ToString() : "Invalid Condition";
}

HTNConditionAnd::HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions)
	: mConditions(inConditions)
{
}

std::string HTNConditionAnd::ToString() const
{
	// TODO salvarez Print names of conditions AND
	return "AND Condition";
}

bool HTNConditionAnd::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
	const std::shared_ptr<const HTNConditionBase> This = shared_from_this();
	for (unsigned int CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentConditionIndex(This); CurrentConditionIndex < mConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentConditionIndex(This))
	{
		const std::shared_ptr<const HTNConditionBase>& CurrentCondition = mConditions[CurrentConditionIndex];
		if (!CurrentCondition->Check(ioDecompositionContext))
		{
			return false;
		}

		// Continue
		CurrentDecomposition.IncrementCurrentConditionIndex(This);
	}

	return true;
}

HTNConditionOr::HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions)
	: mConditions(inConditions)
{
}

std::string HTNConditionOr::ToString() const
{
	// TODO salvarez Print names of conditions OR
	return "OR Condition";
}

bool HTNConditionOr::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
	const std::shared_ptr<const HTNConditionBase> This = shared_from_this();
	for (unsigned int CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentConditionIndex(This); CurrentConditionIndex < mConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentConditionIndex(This))
	{
		const std::shared_ptr<const HTNConditionBase>& CurrentCondition = mConditions[CurrentConditionIndex];
		if (CurrentCondition->Check(ioDecompositionContext))
		{
			return true;
		}

		// Continue
		CurrentDecomposition.IncrementCurrentConditionIndex(This);
	}

	return false;
}

HTNConditionNot::HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inCondition)
	: mCondition(inCondition)
{
}

std::string HTNConditionNot::ToString() const
{
	// TODO salvarez Print names of conditions !
	return "NOT Condition";
}

bool HTNConditionNot::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	// TODO salvarez Fix backtracking
	return !mCondition->Check(ioDecompositionContext);
}
