// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Planner/HTNPlanningUnit.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"

HTNPlanningUnit::HTNPlanningUnit(const std::string& inID, const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook)
    : mID(inID), mDatabaseHook(inDatabaseHook), mPlannerHook(inPlannerHook)
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
    mLastDecompositionResult = DecompositionContext.GetDecompositionResult();
#endif

    return Result;
}
