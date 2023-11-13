#include "Domain/Interpreter/HTNTaskInstance.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                                 const HTNNodePath& inNodePath, const HTNNodePath& inVariableScopeNodePath)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment), mNodePath(inNodePath), mVariableScopeNodePath(inVariableScopeNodePath)
{
}
