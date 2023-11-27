// Copyright(c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNNodeSnapshot.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeSnapshot::HTNNodeSnapshot(const HTNVariablesManager& inVariablesManager) : mVariablesManager(inVariablesManager)
{
}

HTNNodeSnapshot::HTNNodeSnapshot(const HTNVariablesManager& inVariablesManager, const bool inResult)
    : mVariablesManager(inVariablesManager), mResult(inResult)
{
}
#endif
