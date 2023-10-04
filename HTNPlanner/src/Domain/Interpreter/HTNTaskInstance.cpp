#include "HTNTaskInstance.h"

#include "Domain/AST/HTNTaskNode.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                                 const std::string& inVariableScopePath)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment), mVariableScopePath(inVariableScopePath)
{
}

HTNTaskInstance::~HTNTaskInstance() = default;
