// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNTaskInstance.h"

HTNTaskInstance::HTNTaskInstance(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode, const HTNEnvironment& inEnvironment,
                                 const HTNPathManager& inNodePathManager, const HTNPathManager& inVariablesPathManager)
    : mTaskNode(inTaskNode), mEnvironment(inEnvironment), mNodePathManager(inNodePathManager), mVariablesPathManager(inVariablesPathManager)
{
}
