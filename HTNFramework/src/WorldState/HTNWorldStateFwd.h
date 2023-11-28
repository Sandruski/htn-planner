// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "WorldState/HTNWorldStateHelpers.h"

class HTNFactArgumentsTable;
class HTNWorldState;

using HTNFactArguments       = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFacts               = std::unordered_map<std::string, HTNFactArgumentsTables>;
using HTNFact                = std::pair<const std::string, HTNFactArgumentsTables>;
