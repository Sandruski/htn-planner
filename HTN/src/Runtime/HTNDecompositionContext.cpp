#include "Runtime/HTNDecompositionContext.h"

#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNNodeBase.h"

#include <format>

const std::shared_ptr<const HTNDomain> HTNDecompositionContext::GetCurrentDomain() const
{
    // First element is always a domain
    return !mCurrentScope.empty() ? std::static_pointer_cast<const HTNDomain>(mCurrentScope[0]) : nullptr;
}

const std::shared_ptr<const HTNMethod> HTNDecompositionContext::GetCurrentMethod() const
{
    // Second element is always a method
    return (mCurrentScope.size() >= 2) ? std::static_pointer_cast<const HTNMethod>(mCurrentScope[1]) : nullptr;
}

std::string HTNDecompositionContext::MakeCurrentScopeID() const
{
    std::string CurrentScopeID;

    for (const std::shared_ptr<const HTNNodeBase>& Node : mCurrentScope)
    {
        CurrentScopeID.append(std::format("{}_", Node->GetID()));
    }

    // Remove last "_"
    const size_t Position = CurrentScopeID.find_last_of("_");
    if (Position != std::string::npos)
    {
        CurrentScopeID.erase(Position);
    }

    return CurrentScopeID;
}

bool HTNDecompositionContext::RestoreDecomposition()
{
    if (mDecompositionHistory.empty())
    {
        mCurrentDecomposition = HTNDecompositionRecord();
        return false;
    }

    mCurrentDecomposition = mDecompositionHistory.back();
    mDecompositionHistory.pop_back();

    return true;
}
