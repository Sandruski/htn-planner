// Copyright(c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNNodeResult.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeResult::HTNNodeResult(const HTNVariablesManager& inVariablesManager) : mVariablesManager(inVariablesManager)
{
}

HTNNodeResult::HTNNodeResult(const HTNVariablesManager& inVariablesManager, const bool inResult)
    : mVariablesManager(inVariablesManager), mResult(inResult)
{
}
#endif
