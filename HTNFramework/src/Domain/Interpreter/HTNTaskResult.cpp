// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNTaskResult.h"

HTNTaskResult::HTNTaskResult(const std::string& inTaskID, const std::vector<HTNAtom>& inTaskArguments)
    : mTaskID(inTaskID), mTaskArguments(inTaskArguments)
{
}
