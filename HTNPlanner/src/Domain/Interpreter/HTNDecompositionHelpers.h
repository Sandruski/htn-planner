#pragma once

#include "HTNPlannerMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDecompositionHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

bool IsParameter(const std::string& inVariableID);
bool IsAnyArgument(const std::string& inVariableID);

// ID of the top-level compound task node
constexpr uint32 kTopLevelCompoundTaskNodeID = 0;

const std::string kDefaultMainTopLevelMethodID      = "behave";
const std::string kDefaultUpperBodyTopLevelMethodID = "behave_upper_body";

const std::string kInputParameterPrefix       = "inp_";
const std::string kOutputParameterPrefix      = "out_";
const std::string kInputOutputParameterPrefix = "io_";
const std::string kAnyArgumentPrefix          = "any_";
} // namespace HTNDecompositionHelpers
