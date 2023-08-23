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

std::string HTNConditionBase::GetID() const
{
    return std::to_string(mID);
}

HTNCondition::HTNCondition(const std::vector<std::shared_ptr<const HTNValue>>& inArguments) : mArguments(inArguments)
{
}

std::string HTNCondition::ToString() const
{
    std::string Description;

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Description.append(std::format("{} ", Argument->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}

bool HTNCondition::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    const std::string       ID                   = GetID();
    const std::string       FactID               = GetIDArgument()->ToString();
    const std::string       CurrentScopeID       = ioDecompositionContext.MakeCurrentScopeID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();
    const HTNWorldState*    WorldState           = ioDecompositionContext.GetWorldState();

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
            ArgumentValueMutable            = &CurrentDecomposition.GetOrAddVariable(CurrentScopeID, ArgumentValueString);
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
    const HTNConditionWorldStateQuery Condition        = HTNConditionWorldStateQuery(FactID, Arguments);
    const unsigned int                ArgumentsCount   = static_cast<unsigned int>(Arguments.size());
    const unsigned int                FactEntriesCount = WorldState->GetNumFactEntries(FactID, ArgumentsCount);
    for (unsigned int CurrentFactEntryIndex = CurrentDecomposition.AddOrIncrementIndex(ID); CurrentFactEntryIndex < FactEntriesCount;
         CurrentFactEntryIndex              = CurrentDecomposition.AddOrIncrementIndex(ID))
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

HTNConditionAxiom::HTNConditionAxiom(const std::vector<std::shared_ptr<const HTNValue>>& inArguments) : mArguments(inArguments)
{
}

std::string HTNConditionAxiom::ToString() const
{
    std::string Description;

    for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
    {
        Description.append(std::format("{} ", Argument->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}

bool HTNConditionAxiom::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    outHasBoundArguments = false;

    const std::string                       AxiomID       = GetIDArgument()->ToString();
    const std::shared_ptr<const HTNDomain>& CurrentDomain = ioDecompositionContext.GetCurrentDomain();
    const std::shared_ptr<const HTNAxiom>   CurrentAxiom  = CurrentDomain->FindAxiomByID(AxiomID);
    if (!CurrentAxiom)
    {
        LOG_ERROR("Axiom [{}] not found", AxiomID);
        return false;
    }

    // TODO salvarez RELATED Finish this
    const std::string CurrentScopeID = ioDecompositionContext.MakeCurrentScopeID();

    HTNDecompositionScope CurrentAxiomScope   = HTNDecompositionScope(ioDecompositionContext, CurrentAxiom);
    const std::string     CurrentAxiomScopeID = ioDecompositionContext.MakeCurrentScopeID();

    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    // Initialize the input arguments of the axiom with the arguments of the condition
    const std::vector<std::shared_ptr<const HTNValue>>& CurrentAxiomArguments = CurrentAxiom->GetArguments();
    if (!HTN::Helpers::CopyArguments(CurrentDecomposition, mArguments, CurrentAxiomArguments, CurrentScopeID, CurrentAxiomScopeID, {},
                                     HTN::Helpers::InputPrefixes))
    {
        LOG_ERROR("Arguments could not be copied from condition to axiom [{}]", AxiomID);
        return false;
    }

    const std::shared_ptr<const HTNConditionBase>& CurrentCondition = CurrentAxiom->GetCondition();
    if (!CurrentCondition)
    {
        // If empty, it evaluates to true
        return true;
    }

    bool       HasBoundArguments = false;
    const bool Result            = CurrentCondition->Check(ioDecompositionContext, HasBoundArguments);
    if (!Result)
    {
        return false;
    }

    // Copy back the output arguments of the axiom
    if (!HTN::Helpers::CopyArguments(CurrentDecomposition, CurrentAxiomArguments, mArguments, CurrentAxiomScopeID, CurrentScopeID,
                                     HTN::Helpers::OutputPrefixes, {}))
    {
        LOG_ERROR("Arguments could not be copied from axiom to condition [{}]", AxiomID);
        return false;
    }

    return true;
}

HTNConditionAnd::HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions) : mSubConditions(inSubConditions)
{
}

std::string HTNConditionAnd::ToString() const
{
    std::string Description = "and";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Description.append(std::format(" {}", SubCondition->ToString()));
    }

    return Description;
}

bool HTNConditionAnd::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    const std::string       CurrentScopeID       = ioDecompositionContext.MakeCurrentScopeID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex = CurrentDecomposition.GetOrAddIndex(ID); CurrentConditionIndex < mSubConditions.size();
         CurrentConditionIndex              = CurrentDecomposition.GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.HasVariables(CurrentScopeID)
                                                                              ? CurrentDecomposition.GetVariables(CurrentScopeID)
                                                                              : std::unordered_map<std::string, HTNAtom>();

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
            NewDecomposition.SetVariables(CurrentScopeID, CurrentVariables);
            ioDecompositionContext.RecordDecomposition(NewDecomposition);
        }

        // Continue
        CurrentDecomposition.IncrementIndex(ID);
    }

    // If empty, it evaluates to true
    return true;
}

HTNConditionOr::HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions) : mSubConditions(inSubConditions)
{
}

std::string HTNConditionOr::ToString() const
{
    std::string Description = "or";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Description.append(std::format(" {}", SubCondition->ToString()));
    }

    return Description;
}

bool HTNConditionOr::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // NO backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex = CurrentDecomposition.GetOrAddIndex(ID); CurrentConditionIndex < mSubConditions.size();
         CurrentConditionIndex              = CurrentDecomposition.GetOrAddIndex(ID))
    {
        const std::shared_ptr<const HTNConditionBase>& CurrentCondition  = mSubConditions[CurrentConditionIndex];
        bool                                           HasBoundArguments = false;
        if (CurrentCondition->Check(ioDecompositionContext, HasBoundArguments))
        {
            return true;
        }

        // Continue
        CurrentDecomposition.IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return false;
}

HTNConditionAlt::HTNConditionAlt(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions) : mSubConditions(inSubConditions)
{
}

std::string HTNConditionAlt::ToString() const
{
    std::string Description = "alt";

    for (const std::shared_ptr<const HTNConditionBase>& SubCondition : mSubConditions)
    {
        Description.append(std::format(" {}", SubCondition->ToString()));
    }

    return Description;
}

bool HTNConditionAlt::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // YES bindings
    // YES backtracking

    outHasBoundArguments = false;

    const std::string       ID                   = GetID();
    const std::string       CurrentScopeID       = ioDecompositionContext.MakeCurrentScopeID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    for (unsigned int CurrentConditionIndex = CurrentDecomposition.GetOrAddIndex(ID); CurrentConditionIndex < mSubConditions.size();
         CurrentConditionIndex              = CurrentDecomposition.GetOrAddIndex(ID))
    {
        // Copy variables
        const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.HasVariables(CurrentScopeID)
                                                                              ? CurrentDecomposition.GetVariables(CurrentScopeID)
                                                                              : std::unordered_map<std::string, HTNAtom>();

        const std::shared_ptr<const HTNConditionBase>& CurrentCondition  = mSubConditions[CurrentConditionIndex];
        bool                                           HasBoundArguments = false;
        if (CurrentCondition->Check(ioDecompositionContext, HasBoundArguments))
        {
            // Record decomposition if it has bound any arguments
            if (HasBoundArguments)
            {
                HTNDecompositionRecord NewDecomposition = CurrentDecomposition;
                NewDecomposition.SetVariables(CurrentScopeID, CurrentVariables);
                ioDecompositionContext.RecordDecomposition(NewDecomposition);
            }

            return true;
        }

        // Continue
        CurrentDecomposition.IncrementIndex(ID);
    }

    // If empty, it evaluates to false
    return false;
}

HTNConditionNot::HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inSubCondition) : mSubCondition(inSubCondition)
{
}

std::string HTNConditionNot::ToString() const
{
    return std::format("not {}", mSubCondition->ToString());
}

bool HTNConditionNot::Check(HTNDecompositionContext& ioDecompositionContext, bool& outHasBoundArguments) const
{
    // NO bindings
    // NO backtracking

    outHasBoundArguments = false;

    // Copy variables
    const std::string       CurrentScopeID       = ioDecompositionContext.MakeCurrentScopeID();
    HTNDecompositionRecord& CurrentDecomposition = ioDecompositionContext.GetCurrentDecompositionMutable();

    const std::unordered_map<std::string, HTNAtom> CurrentVariables = CurrentDecomposition.HasVariables(CurrentScopeID)
                                                                          ? CurrentDecomposition.GetVariables(CurrentScopeID)
                                                                          : std::unordered_map<std::string, HTNAtom>();

    // Copy decomposition history
    const std::vector<HTNDecompositionRecord> DecompositionHistory = ioDecompositionContext.GetDecompositionHistory();

    // Check condition
    bool       HasBoundArguments = false;
    const bool Result            = mSubCondition->Check(ioDecompositionContext, HasBoundArguments);

    // Reset decomposition history
    ioDecompositionContext.SetDecompositionHistory(DecompositionHistory);

    // Reset variables
    CurrentDecomposition.SetVariables(CurrentScopeID, CurrentVariables);

    // Invert result
    return !Result;
}
