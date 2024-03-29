// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Interpreter/Scope/HTNVariablesScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNVariablesManager.h"

HTNVariablesScope::HTNVariablesScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext)
    : HTNScope(inNodeID, ioDecompositionContext.GetCurrentVariablesPathManagerMutable()),
      mVariablesManager(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetVariablesManagerMutable())
{
}

HTNVariablesScope::~HTNVariablesScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the variables associated to the variables path when the node goes out of scope to minimize memory storage requirements
    const std::string& VariablesPath = mPathManager.GetPath();
    mVariablesManager.RemoveVariables(VariablesPath);
}
