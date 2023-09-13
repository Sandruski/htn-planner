#include "Interpreter/HTNDecompositionScope.h"

#include "Interpreter/HTNDecompositionContext.h"

#include <cassert>

HTNDecompositionScope::HTNDecompositionScope(HTNDecompositionContext& inDecompositionContext, const std::shared_ptr<const HTNNodeBase>& inNode)
    : mDecompositionContext(inDecompositionContext), mNode(inNode)
{
    mDecompositionContext.PushNodeToCurrentScope(mNode);
}

HTNDecompositionScope::~HTNDecompositionScope()
{
    const std::shared_ptr<const HTNNodeBase>& Node = mDecompositionContext.PopNodeFromCurrentScope();
    assert(mNode == Node);
}
