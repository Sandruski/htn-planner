// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Domain/Printer/HTNNodeInstance.h"

#ifdef HTN_DEBUG_DECOMPOSITION
HTNNodeInstance::HTNNodeInstance(const HTNNodeSnapshot& inNodeSnapshot, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeSnapshot&                                               inNodeSnapshot,
                                 const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& inNodeParameters,
                                 const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeParameters(inNodeParameters), mVariablesPathManager(inVariablesPathManager), mNodeLabel(inNodeLabel)
{
}

HTNNodeInstance::HTNNodeInstance(const HTNNodeSnapshot&                                                inNodeSnapshot,
                                 const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& inNodeArguments,
                                 const HTNPathManager& inVariablesPathManager, const std::string& inNodeLabel)
    : mNodeSnapshot(&inNodeSnapshot), mNodeArguments(inNodeArguments), mVariablesPathManager(inVariablesPathManager), mNodeLabel(inNodeLabel)
{
}
#endif
