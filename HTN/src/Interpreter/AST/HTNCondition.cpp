#include "Interpreter/AST/HTNCondition.h"

#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"
#include "Log.h"
#include "Runtime/HTNDecompositionContext.h"

#include <cassert>
#include <format>

bool HTNConditionWorldStateQuery::Check(const HTNWorldState& inWorldState, const int inIndex) const
{
    return inWorldState.CheckIndex(mKey.c_str(), inIndex, mArguments);
}

std::vector<std::shared_ptr<const HTNTask>> HTNConditionBase::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

HTNCondition::HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments,
                           const bool inIsAxiom)
    : mName(std::move(inName)), mArguments(inArguments), mIsAxiom(inIsAxiom)
{
}

std::string HTNCondition::ToString() const
{
    std::string Name = GetName();

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Name.append(std::format(" {}", Argument->ToString()));
    }

    return Name;
}

bool HTNCondition::Check(HTNDecompositionContext& ioDecompositionContext) const
{
    HTNDecompositionRecord&                 CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNMethod>& CurrentMethod        = ioDecompositionContext.GetCurrentMethod();

    if (mIsAxiom)
    {
        const std::shared_ptr<const HTNDomain>& Domain = CurrentMethod->GetParent().lock();
        const std::shared_ptr<const HTNAxiom>   Axiom  = Domain->FindAxiomByName(GetName());
        if (!Axiom)
        {
            LOG_ERROR("Axiom [{}] not found", GetName());
            return false;
        }

        const std::shared_ptr<const HTNConditionBase>& Condition = Axiom->GetCondition();
        if (!Condition)
        {
            return true;
        }

        return Condition->Check(ioDecompositionContext);
    }

    const std::shared_ptr<const HTNCondition> This       = shared_from_this();
    const HTNWorldState*                      WorldState = ioDecompositionContext.GetWorldState();

    const HTNAtom&     NameValue = mName->GetValue();
    const std::string& Key       = NameValue.GetValue<std::string>();

    // Add variables for the new arguments
    std::vector<HTNAtom*> Arguments;
    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        const HTNAtom&     ArgumentValue = Argument->GetValue();
        const std::string& VariableName  = ArgumentValue.GetValue<std::string>();
        HTNAtom&           Variable      = CurrentDecomposition.GetOrAddVariable(CurrentMethod, VariableName);
        Arguments.emplace_back(&Variable);
    }

    // Try to bind the variables
    const HTNConditionWorldStateQuery Condition        = HTNConditionWorldStateQuery(Key, Arguments);
    const unsigned int                ArgumentsCount   = static_cast<unsigned int>(Arguments.size());
    const unsigned int                FactEntriesCount = WorldState->GetNumFactEntries(Key, ArgumentsCount);
    for (unsigned int CurrentFactEntryIndex                              = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This);
         CurrentFactEntryIndex < FactEntriesCount; CurrentFactEntryIndex = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This))
    {
        if (Condition.Check(*WorldState, CurrentFactEntryIndex))
        {
            return true;
        }
    }

    return false;
}

std::string HTNCondition::GetName() const
{
    return mName ? mName->ToString() : "Invalid Condition";
}

std::string HTNConditionAnd::ToString() const
{
    std::string Name = "AND";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Name.append(std::format(" {}", SubCondition->ToString()));
    }

    return Name;
}

bool HTNConditionAnd::Check(HTNDecompositionContext& ioDecompositionContext) const
{
    // YES bindings
    // YES backtracking

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNMethod>&       CurrentMethod        = ioDecompositionContext.GetCurrentMethod();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> VariablesBefore = CurrentDecomposition.HasVariables(CurrentMethod)
                                                                             ? CurrentDecomposition.GetVariables(CurrentMethod)
                                                                             : std::unordered_map<std::string, HTNAtom>();

        // Check condition
        const std::shared_ptr<const HTNConditionBase>& CurrentCondition = mSubConditions[CurrentConditionIndex];
        if (CurrentCondition->Check(ioDecompositionContext))
        {
            // Continue
            CurrentDecomposition.IncrementCurrentSubConditionIndex(This);

            // Record decomposition if it has bound any unbound arguments
            const std::unordered_map<std::string, HTNAtom> VariablesAfter = CurrentDecomposition.HasVariables(CurrentMethod)
                                                                                ? CurrentDecomposition.GetVariables(CurrentMethod)
                                                                                : std::unordered_map<std::string, HTNAtom>();
            if (VariablesAfter.size() > VariablesBefore.size()) // TODO salvarez Make sure that this works
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                NewDecomposition.SetVariables(CurrentMethod, VariablesBefore);
                ioDecompositionContext.RecordDecomposition(NewDecomposition);
            }
        }
        else
        {
            if (CurrentConditionIndex == 0) // First condition
            {
                return false;
            }
            else
            {
                // Restore state: unbound variables but updated indices
                ioDecompositionContext.RestoreDecomposition();
            }
        }
    }

    // If empty, it evaluates to true
    return true;
}

std::string HTNConditionOr::ToString() const
{
    std::string Name = "OR";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Name.append(std::format(" {}", SubCondition->ToString()));
    }

    return Name;
}

bool HTNConditionOr::Check(HTNDecompositionContext& ioDecompositionContext) const
{
    // YES bindings
    // NO backtracking

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        const std::shared_ptr<const HTNConditionBase>& CurrentCondition = mSubConditions[CurrentConditionIndex];
        if (CurrentCondition->Check(ioDecompositionContext))
        {
            return true;
        }
        else
        {
            // Continue
            CurrentDecomposition.IncrementCurrentSubConditionIndex(This);
        }
    }

    // If empty, it evaluates to false
    return false;
}

std::string HTNConditionAlt::ToString() const
{
    std::string Name = "ALT";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Name.append(std::format(" {}", SubCondition->ToString()));
    }

    return Name;
}

bool HTNConditionAlt::Check(HTNDecompositionContext& ioDecompositionContext) const
{
    // YES bindings
    // YES backtracking

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNMethod>&       CurrentMethod        = ioDecompositionContext.GetCurrentMethod();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> VariablesBefore = CurrentDecomposition.HasVariables(CurrentMethod)
                                                                             ? CurrentDecomposition.GetVariables(CurrentMethod)
                                                                             : std::unordered_map<std::string, HTNAtom>();

        const std::shared_ptr<const HTNConditionBase>& CurrentCondition = mSubConditions[CurrentConditionIndex];
        if (CurrentCondition->Check(ioDecompositionContext))
        {
            // Record decomposition if it has bound any unbound arguments
            const std::unordered_map<std::string, HTNAtom> VariablesAfter = CurrentDecomposition.HasVariables(CurrentMethod)
                                                                                ? CurrentDecomposition.GetVariables(CurrentMethod)
                                                                                : std::unordered_map<std::string, HTNAtom>();
            if (VariablesAfter.size() > VariablesBefore.size()) // TODO salvarez Make sure that this works
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                NewDecomposition.SetVariables(CurrentMethod, VariablesBefore);
                ioDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            return true;
        }
        else
        {
            // Continue
            CurrentDecomposition.IncrementCurrentSubConditionIndex(This);
        }
    }

    // If empty, it evaluates to false
    return false;
}

std::string HTNConditionNot::ToString() const
{
    return std::format("NOT {}", mSubCondition->ToString());
}

bool HTNConditionNot::Check(HTNDecompositionContext& ioDecompositionContext) const
{
    // NO bindings
    // NO backtracking

    // Copy variables
    HTNDecompositionRecord&                        CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNMethod>&        CurrentMethod        = ioDecompositionContext.GetCurrentMethod();
    const std::unordered_map<std::string, HTNAtom> Variables            = CurrentDecomposition.HasVariables(CurrentMethod)
                                                                              ? CurrentDecomposition.GetVariables(CurrentMethod)
                                                                              : std::unordered_map<std::string, HTNAtom>();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = ioDecompositionContext.GetDecompositionHistory();

    // Check condition
    const bool Result = mSubCondition->Check(ioDecompositionContext);

    // Reset decomposition history
    ioDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.SetVariables(CurrentMethod, Variables);

    // Invert result
    return !Result;
}
