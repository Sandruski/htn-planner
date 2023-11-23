// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Helpers/HTNPlanningQuery.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Helpers/HTNOperationResultHelpers.h"

HTNPlanningQuery::HTNPlanningQuery(HTNPlanningUnit& inPlanningUnit) : mPlanningUnit(inPlanningUnit)
{
}

bool HTNPlanningQuery::IsLastDecompositionSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastDecompositionResult);
}
#endif
