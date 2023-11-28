// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"

#include <string>
#include <unordered_map>

class HTNChoicePointNodeState;
class HTNNodeState;
class HTNNodeStateBase;

// Node path to node state
using HTNNodeStates = std::unordered_map<std::string, HTNNodeState>;

// Node path to choice point node state
using HTNChoicePointNodeStates = std::unordered_map<std::string, HTNChoicePointNodeState>;
#endif
