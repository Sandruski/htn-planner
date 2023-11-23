// Copyright(c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNNodeResult.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeResult::HTNNodeResult(const HTNVariables& inVariables) : mVariables(inVariables)
{
}

HTNNodeResult::HTNNodeResult(const HTNVariables& inVariables, const bool inResult) : mVariables(inVariables), mResult(inResult)
{
}
#endif
