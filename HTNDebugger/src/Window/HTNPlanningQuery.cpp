// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Window/HTNPlanningQuery.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Window/HTNOperationResultHelpers.h"

HTNPlanningQuery::HTNPlanningQuery(HTNPlanningUnit& ioPlanningUnit) : mPlanningUnit(ioPlanningUnit)
{
}

bool HTNPlanningQuery::IsLastDecomposeDomainOperationSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastDecomposeDomainOperationResult);
}
#endif