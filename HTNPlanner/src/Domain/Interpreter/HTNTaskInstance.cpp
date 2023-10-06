#include "HTNTaskInstance.h"

#include "Domain/Nodes/HTNTaskNode.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                                 const std::string& inNodePath, const std::string& inVariableScopePath)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment), mNodePath(inNodePath), mVariableScopePath(inVariableScopePath)
{
}

HTNTaskInstance::~HTNTaskInstance() = default;
