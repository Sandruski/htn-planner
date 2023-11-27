// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Hook/HTNPlanningUnit.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Hook/HTNDatabaseHook.h"
#include "Hook/HTNPlannerHook.h"

HTNPlanningUnit::HTNPlanningUnit(const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook,
                                 const std::string& inDefaultTopLevelMethodID)
    : mDatabaseHook(inDatabaseHook), mPlannerHook(inPlannerHook), mDefaultTopLevelMethodID(inDefaultTopLevelMethodID)
{
}

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointID)
{
    const HTNWorldState&                        WorldState           = mDatabaseHook.GetWorldState();
    const std::shared_ptr<const HTNDomainNode>& DomainNode           = mPlannerHook.GetDomainNode();
    HTNDecompositionContext                     DecompositionContext = HTNDecompositionContext(WorldState, DomainNode, inEntryPointID);
    const bool                                  Result               = mPlannerHook.MakePlan(DecompositionContext);

    mLastDecomposition = DecompositionContext.GetCurrentDecomposition();
#ifdef HTN_DEBUG_DECOMPOSITION
    mLastDecompositionSnapshot = DecompositionContext.GetDecompositionSnapshot();
#endif

    return Result;
}

bool HTNPlanningUnit::ExecuteTopLevelMethod()
{
    return ExecuteTopLevelMethod(mDefaultTopLevelMethodID);
}
