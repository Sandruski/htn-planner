// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Helpers/HTNPlanningQuery.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Helpers/HTNOperationResultHelpers.h"

HTNPlanningQuery::HTNPlanningQuery(HTNPlanningUnit& ioPlanningUnit) : mPlanningUnit(ioPlanningUnit)
{
}

bool HTNPlanningQuery::IsLastDecompositionOperationSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastDecompositionOperationResult);
}
#endif
