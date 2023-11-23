// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDomainHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

bool IsInputParameter(const std::string& inVariableID);
bool IsOutputParameter(const std::string& inVariableID);
bool IsInputOutputParameter(const std::string& inVariableID);
bool IsParameter(const std::string& inVariableID);
bool IsAnyArgument(const std::string& inVariableID);

inline const std::string kDefaultMainTopLevelMethodID      = "behave";
inline const std::string kDefaultUpperBodyTopLevelMethodID = "behave_upper_body";
inline const std::string kAnonymousNamespaceID             = "unnamed";
} // namespace HTNDomainHelpers
