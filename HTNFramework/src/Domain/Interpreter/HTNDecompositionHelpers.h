// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDecompositionHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

bool IsInputParameter(const std::string& inVariableID);
bool IsOutputParameter(const std::string& inVariableID);
bool IsInputOutputParameter(const std::string& inVariableID);
bool IsParameter(const std::string& inVariableID);
bool IsAnyArgument(const std::string& inVariableID);

// ID of the top-level compound task node
inline constexpr uint32 kTopLevelCompoundTaskNodeID = 0;

inline const std::string kDefaultMainTopLevelMethodID      = "behave";
inline const std::string kDefaultUpperBodyTopLevelMethodID = "behave_upper_body";

inline const std::string kInputParameterPrefix       = "inp_";
inline const std::string kOutputParameterPrefix      = "out_";
inline const std::string kInputOutputParameterPrefix = "io_";
inline const std::string kAnyArgumentPrefix          = "any_";
} // namespace HTNDecompositionHelpers
