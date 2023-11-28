// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Window/HTNPlanningRequest.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Window/HTNOperationResultHelpers.h"

HTNPlanningRequest::HTNPlanningRequest(HTNPlanningUnit& ioPlanningUnit) : mPlanningUnit(ioPlanningUnit)
{
}

bool HTNPlanningRequest::IsLastDecomposeDomainOperationSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastDecomposeDomainOperationResult);
}
#endif
