// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/Scope/HTNNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNIndicesManager.h"

HTNNodeScope::HTNNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext)
    : HTNScope(inNodeID, ioDecompositionContext.GetCurrentNodePathManagerMutable()),
      mIndicesManager(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetIndicesManagerMutable())
{
}

HTNNodeScope::~HTNNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the index associated to the node path when the node goes out of scope to minimize memory storage requirements
    const std::string& NodePath = mPathManager.GetPath();
    mIndicesManager.RemoveIndex(NodePath);
}
