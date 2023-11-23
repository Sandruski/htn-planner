// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNTaskInstance.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                                 const HTNPathHandler& inNodePathHandler, const HTNPathHandler& inVariablesPathHandler)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment), mNodePathHandler(inNodePathHandler), mVariablesPathHandler(inVariablesPathHandler)
{
}
