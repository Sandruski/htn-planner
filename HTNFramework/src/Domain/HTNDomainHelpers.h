// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDomainHelpers
{
HTN_NODISCARD std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

HTN_NODISCARD bool IsInputParameter(const std::string& inVariableID);
HTN_NODISCARD bool IsOutputParameter(const std::string& inVariableID);
HTN_NODISCARD bool IsInputOutputParameter(const std::string& inVariableID);
HTN_NODISCARD bool IsParameter(const std::string& inVariableID);
HTN_NODISCARD bool IsAnyArgument(const std::string& inVariableID);

inline const std::string kAnonymousNamespaceID             = "unnamed";
inline const std::string kTopLevelDomainID                 = "top_level_domain";
inline const std::string kTopLevelMethodID                 = "top_level_method";
} // namespace HTNDomainHelpers
