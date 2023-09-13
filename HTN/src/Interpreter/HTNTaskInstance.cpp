#include "HTNTaskInstance.h"

#include "Parser/AST/HTNTaskNode.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNode>& inTaskNode, const std::string& inScopeID)
    : mTaskNode(inTaskNode), mScopeID(inScopeID)
{
}

HTNTaskInstance::~HTNTaskInstance() = default;
