// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNNodeInstance.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeInstance::HTNNodeInstance(const HTNNodeResult& inNodeResult, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeResult&                                                 inNodeResult,
                                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                           const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeParameters(inNodeParameters), mVariablesPathManager(inVariablesPathManager), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeResult&                                                  inNodeResult,
                                           const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                           const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeArguments(inNodeArguments), mVariablesPathManager(inVariablesPathManager), mNodeLabel(inNodeLabel)
{
}
#endif
