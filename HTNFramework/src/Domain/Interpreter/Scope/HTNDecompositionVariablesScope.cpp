// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/Scope/HTNDecompositionVariablesScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNVariables.h"

HTNDecompositionVariablesScope::HTNDecompositionVariablesScope(const std::string& inNodeID,
                                                                               HTNDecompositionContext& ioDecompositionContext)
    : HTNScope(inNodeID, ioDecompositionContext.GetCurrentVariablesPathManagerMutable()),
      mVariables(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetVariablesMutable())
{
}

HTNDecompositionVariablesScope::~HTNDecompositionVariablesScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the variables associated to the node path when the node goes out of scope to minimize memory storage requirements
    const std::string& NodePath = mPathManager.GetPath();
    mVariables.RemoveVariables(NodePath);
}
