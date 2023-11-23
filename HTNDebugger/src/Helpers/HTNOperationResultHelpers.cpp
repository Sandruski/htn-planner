// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Helpers/HTNOperationResultHelpers.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Helpers/HTNOperationResult.h"

namespace HTNOperationResultHelpers
{
bool IsOperationSuccessful(const HTNOperationResult inOperationResult)
{
    return (inOperationResult == HTNOperationResult::SUCCEEDED);
}
} // namespace HTNOperationResultHelpers
#endif
