#include "Domain/Interpreter/HTNDecompositionVariableScopeNodeScope.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNVariables.h"

HTNDecompositionVariableScopeNodeScope::HTNDecompositionVariableScopeNodeScope(const std::string&       inNodeID,
                                                                               HTNDecompositionContext& ioDecompositionContext)
    : HTNNodeScope(inNodeID, ioDecompositionContext.GetCurrentVariableScopeNodePathMutable()),
      mVariables(ioDecompositionContext.GetCurrentDecompositionMutable().GetEnvironmentMutable().GetVariablesMutable())
{
}

HTNDecompositionVariableScopeNodeScope::~HTNDecompositionVariableScopeNodeScope()
{
    if (!mResult)
    {
        return;
    }

    // Remove the variables associated to the node path when the node goes out of scope to minimize memory storage requirements
    const std::string& NodePath = mNodePath.GetNodePath();
    mVariables.RemoveVariables(NodePath);
}
