#include "HTNTaskInstance.h"

#include "Interpreter/AST/HTNTask.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTask>& inTask, const std::string& inScopeID) : mTask(inTask), mScopeID(inScopeID)
{
}

HTNTaskInstance::~HTNTaskInstance() = default;
