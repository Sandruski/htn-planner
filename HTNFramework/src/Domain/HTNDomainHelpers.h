// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDomainHelpers
{
// Returns a compound task node for the given method ID
HTN_NODISCARD std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inMethodID);

// Returns whether the variable ID represents an input parameter
HTN_NODISCARD bool IsInputParameter(const std::string& inVariableID);

// Returns whether the variable ID represents an output parameter
HTN_NODISCARD bool IsOutputParameter(const std::string& inVariableID);

// Returns whether the variable ID represents an input/output parameter
HTN_NODISCARD bool IsInputOutputParameter(const std::string& inVariableID);

// Returns whether the variable ID represents a parameter
HTN_NODISCARD bool IsParameter(const std::string& inVariableID);

// Returns whether the variable ID represents any argument
HTN_NODISCARD bool IsAnyArgument(const std::string& inVariableID);

inline const std::string kAnonymousNamespaceID = "unnamed";
inline const std::string kTopLevelDomainID     = "top_level_domain";
inline const std::string kTopLevelMethodID     = "top_level_method";
} // namespace HTNDomainHelpers
