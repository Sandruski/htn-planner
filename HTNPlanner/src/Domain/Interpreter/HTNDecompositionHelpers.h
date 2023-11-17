#pragma once

#include "HTNPlannerMinimal.h"

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDecompositionHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

bool MakeVariablePath(const std::string& inVariableID, const std::string& inVariableScopeNodePath, std::string& outVariablePath);

bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath);
bool TryPopSegmentFromPath(std::string& ioPath);

bool FindVariableID(const std::string& inVariablePath, std::string& outVariableID);

bool IsParameter(const std::string& inVariableID);
bool IsAnyArgument(const std::string& inVariableID);

// ID of the top-level compound task node
constexpr uint32 kTopLevelCompoundTaskNodeID = 0;

const std::string kDefaultMainTopLevelMethodID      = "behave";
const std::string kDefaultUpperBodyTopLevelMethodID = "behave_upper_body";

const std::string kPathSegmentSeparator = " ";

const std::string kInputParameterPrefix       = "inp_";
const std::string kOutputParameterPrefix      = "out_";
const std::string kInputOutputParameterPrefix = "io_";
const std::string kAnyArgumentPrefix          = "any_";
} // namespace HTNDecompositionHelpers
