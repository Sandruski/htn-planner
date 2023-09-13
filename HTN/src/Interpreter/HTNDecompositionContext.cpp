#include "Interpreter/HTNDecompositionContext.h"

#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNNodeBase.h"

#include <format>

const std::shared_ptr<const HTNDomainNode> HTNDecompositionContext::GetCurrentDomainNode() const
{
    // First element is always a domain
    return !mCurrentScope.empty() ? std::static_pointer_cast<const HTNDomainNode>(mCurrentScope[0]) : nullptr;
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
