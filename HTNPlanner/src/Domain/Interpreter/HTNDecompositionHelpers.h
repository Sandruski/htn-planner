#pragma once

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

// Name of the default top-level method of a top-level domain
const std::string kDefaultTopLevelMethodID = "behave";

// ID of the top-level compound task node
constexpr unsigned int kTopLevelCompoundTaskNodeID = 0;

const std::string kPathSegmentSeparator = " ";
} // namespace HTNDomainHelpers
