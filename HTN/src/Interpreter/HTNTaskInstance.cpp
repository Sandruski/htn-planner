#include "HTNTaskInstance.h"

#include "Domain/AST/HTNTaskNode.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment)
{
}

HTNTaskInstance::~HTNTaskInstance() = default;
