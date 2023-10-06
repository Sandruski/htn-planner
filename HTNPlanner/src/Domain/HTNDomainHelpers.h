#pragma once

#include <memory>
#include <string>

class HTNCompoundTaskNode;

namespace HTNDomainHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID);

// Name of the default top-level method of a top-level domain
const std::string kDefaultTopLevelMethodName = "behave";
} // namespace HTNDomainHelpers
