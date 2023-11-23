// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/Scope/HTNDecompositionNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNIndices.h"

HTNDecompositionNodeScope::HTNDecompositionNodeScope(const std::string& inNodeID, HTNDecompositionContext& ioDecompositionContext)
    : HTNScope(inNodeID, ioDecompositionContext.GetCurrentNodePathHandlerMutable()),
      mIndices(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetIndicesMutable())
{
}

HTNDecompositionNodeScope::~HTNDecompositionNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the index associated to the node path when the node goes out of scope to minimize memory storage requirements
    const std::string& NodePath = mPathHandler.GetPath();
    mIndices.RemoveIndex(NodePath);
}
