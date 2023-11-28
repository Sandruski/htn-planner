// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class HTNAtom;
class HTNFactArgumentsTable;
class HTNWorldState;

using HTNFactArguments           = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsSize>;
using HTNFactArgumentsCollection = std::vector<HTNFactArguments>;

using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsSize>;

// Fact ID to fact argument tables
using HTNFacts = std::unordered_map<std::string, HTNFactArgumentsTables>;
using HTNFact  = std::pair<const std::string, HTNFactArgumentsTables>;
