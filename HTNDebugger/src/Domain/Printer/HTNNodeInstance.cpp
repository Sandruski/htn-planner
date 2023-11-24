// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNNodeInstance.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeInstance::HTNNodeInstance(const HTNNodeResult& inNodeResult, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeResult&                                                 inNodeResult,
                                           const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                           const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeParameters(inNodeParameters), mVariablesPathHandler(inVariablesPathHandler), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeResult&                                                  inNodeResult,
                                           const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                           const HTNPathHandler& inVariablesPathHandler, const std::string& inNodeLabel)
    : mNodeResult(&inNodeResult), mNodeArguments(inNodeArguments), mVariablesPathHandler(inVariablesPathHandler), mNodeLabel(inNodeLabel)
{
}
#endif
