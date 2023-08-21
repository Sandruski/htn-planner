#include "Interpreter/AST/HTNCondition.h"

#include "HTNHelpers.h"
#include "HTNLog.h"
#include "Interpreter/AST/HTNAxiom.h"
#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"
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

HTNCondition::HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
    : mName(std::move(inName)), mArguments(inArguments)
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

bool HTNCondition::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    HTNDecompositionRecord&                   CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNNodeBase>  Scope                = GetScope();
    const std::shared_ptr<const HTNCondition> This                 = shared_from_this();
    const HTNWorldState*                      WorldState           = ioDecompositionContext.GetWorldState();

    const HTNAtom&     NameValue = mName->GetValue();
    const std::string& Key       = NameValue.GetValue<std::string>();

    // Gather arguments
    std::vector<HTNAtom*> Arguments;
    std::vector<HTNAtom>  Literals;
    bool                  ShouldBindArguments = false;
    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        const HTNAtom* ArgumentValue        = &Argument->GetValue();
        HTNAtom*       ArgumentValueMutable = nullptr;

        // TODO salvarez
        // Arguments can be identifiers or literals
        if (Argument->GetType() == HTNValueType::VARIABLE)
        {
            // If argument is a variable, get or add a variable for it in its scope
            std::string ArgumentValueString = ArgumentValue->GetValue<std::string>();
            ArgumentValueMutable            = &CurrentDecomposition.GetOrAddVariable(Scope, ArgumentValueString);
            ShouldBindArguments             = ShouldBindArguments || !ArgumentValueMutable->IsSet();
        }
        else
        {
            // If argument is a literal, create a temporary copy for it
            ArgumentValueMutable = &Literals.emplace_back(*ArgumentValue);
        }

        Arguments.emplace_back(ArgumentValueMutable);
    }

    // Check bound arguments and bind unbound arguments
    const HTNConditionWorldStateQuery Condition        = HTNConditionWorldStateQuery(Key, Arguments);
    const unsigned int                ArgumentsCount   = static_cast<unsigned int>(Arguments.size());
    const unsigned int                FactEntriesCount = WorldState->GetNumFactEntries(Key, ArgumentsCount);
    for (unsigned int CurrentFactEntryIndex                              = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This);
         CurrentFactEntryIndex < FactEntriesCount; CurrentFactEntryIndex = CurrentDecomposition.AddOrIncrementCurrentFactEntryIndex(This))
    {
        if (Condition.Check(*WorldState, CurrentFactEntryIndex))
        {
            outHasBoundArguments = ShouldBindArguments;

            return true;
        }
    }

    outHasBoundArguments = false;

    return false;
}

std::string HTNCondition::GetName() const
{
    return mName->ToString();
}

HTNAxiomCondition::HTNAxiomCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
    : mName(std::move(inName)), mArguments(inArguments)
{
}

std::string HTNAxiomCondition::ToString() const
{
    std::string Name = GetName();

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Name.append(std::format(" {}", Argument->ToString()));
    }

    return Name;
}

bool HTNAxiomCondition::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    outHasBoundArguments = false;

    const std::shared_ptr<const HTNDomain>& Domain = ioDecompositionContext.GetDomain();
    const std::string                       Name   = GetName();
    const std::shared_ptr<const HTNAxiom>   Axiom  = Domain->FindAxiomByName(Name);
    if (!Axiom)
    {
        LOG_ERROR("Axiom [{}] not found", Name);
        return false;
    }

    HTNDecompositionRecord&                  CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNNodeBase> Scope                = GetScope();

    // Initialize the input arguments of the axiom with the arguments of the condition
    const std::vector<std::shared_ptr<const HTNValue>>& AxiomArguments = Axiom->GetArguments();
    if (!HTN::Helpers::CopyArguments(CurrentDecomposition, mArguments, AxiomArguments, Scope, Axiom, {}, HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from condition to axiom [{}]", Name);
        return false;
    }

    const std::shared_ptr<const HTNConditionBase>& Condition = Axiom->GetCondition();
    if (!Condition)
    {
        // If empty, it evaluates to true
        return true;
    }

    bool       HasBoundArguments = false;
    const bool Result            = Condition->Check(ioDecompositionContext, HasBoundArguments);
    if (!Result)
    {
        return false;
    }

    // Copy back the output arguments of the axiom
    if (!HTN::Helpers::CopyArguments(CurrentDecomposition, AxiomArguments, mArguments, Axiom, Scope, HTN::Helpers::OutputPrefixes, {}))
    {
        LOG_ERROR("Arguments could not be copied from axiom to condition [{}]", GetName());
        return false;
    }

    return true;
}

std::string HTNAxiomCondition::GetName() const
{
    return mName->ToString();
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

bool HTNConditionAnd::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNNodeBase>      Scope                = GetScope();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables =
            CurrentDecomposition.HasVariables(Scope) ? CurrentDecomposition.GetVariables(Scope) : std::unordered_map<std::string, HTNAtom>();

        // Check condition
        const std::shared_ptr<const HTNConditionBase>& CurrentCondition  = mSubConditions[CurrentConditionIndex];
        bool                                           HasBoundArguments = false;
        if (!CurrentCondition->Check(ioDecompositionContext, HasBoundArguments))
        {
            if (CurrentConditionIndex == 0) // First condition
            {
                return false;
            }
            else
            {
                // Restore state: unbound variables but updated indices
                ioDecompositionContext.RestoreDecomposition();
                continue;
            }
        }

        // Record decomposition if it has bound any arguments
        if (HasBoundArguments)
        {
            HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
            NewDecomposition.SetVariables(Scope, CurrentVariables);
            ioDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        CurrentDecomposition.IncrementCurrentSubConditionIndex(This);
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

bool HTNConditionOr::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // NO backtracking

    outHasBoundArguments = false;

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        const std::shared_ptr<const HTNConditionBase>& CurrentCondition  = mSubConditions[CurrentConditionIndex];
        bool                                           HasBoundArguments = false;
        if (CurrentCondition->Check(ioDecompositionContext, HasBoundArguments))
        {
            return true;
        }

        // Continue
        CurrentDecomposition.IncrementCurrentSubConditionIndex(This);
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

bool HTNConditionAlt::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    HTNDecompositionRecord&                       CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNNodeBase>      Scope                = GetScope();
    const std::shared_ptr<const HTNConditionBase> This                 = shared_from_this();

    for (unsigned int CurrentConditionIndex                                   = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This);
         CurrentConditionIndex < mSubConditions.size(); CurrentConditionIndex = CurrentDecomposition.GetOrAddCurrentSubConditionIndex(This))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables =
            CurrentDecomposition.HasVariables(Scope) ? CurrentDecomposition.GetVariables(Scope) : std::unordered_map<std::string, HTNAtom>();

        const std::shared_ptr<const HTNConditionBase>& CurrentCondition  = mSubConditions[CurrentConditionIndex];
        bool                                           HasBoundArguments = false;
        if (CurrentCondition->Check(ioDecompositionContext, HasBoundArguments))
        {
            // Record decomposition if it has bound any arguments
            if (HasBoundArguments)
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                NewDecomposition.SetVariables(Scope, CurrentVariables);
                ioDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            return true;
        }

        // Continue
        CurrentDecomposition.IncrementCurrentSubConditionIndex(This);
    }

    // If empty, it evaluates to false
    return false;
}

std::string HTNConditionNot::ToString() const
{
    return std::format("NOT {}", mSubCondition->ToString());
}

bool HTNConditionNot::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // NO bindings
    // NO backtracking

    outHasBoundArguments = false;

    // Copy variables
    HTNDecompositionRecord&                  CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const std::shared_ptr<const HTNNodeBase> Scope                = GetScope();

    const std::unordered_map<std::string, HTNAtom> CurrentVariables =
        CurrentDecomposition.HasVariables(Scope) ? CurrentDecomposition.GetVariables(Scope) : std::unordered_map<std::string, HTNAtom>();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = ioDecompositionContext.GetDecompositionHistory();

    // Check condition
    bool       HasBoundArguments = false;
    const bool Result            = mSubCondition->Check(ioDecompositionContext, HasBoundArguments);

    // Reset decomposition history
    ioDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.SetVariables(Scope, CurrentVariables);

    // Invert result
    return !Result;
}
