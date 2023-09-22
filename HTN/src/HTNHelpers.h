#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDomainNode;
class HTNEnvironment;
class HTNValueNode;
class HTNValueNodeBase;

namespace HTN::Helpers
{
bool CopyArgumentsNoConst(const std::shared_ptr<const HTNDomainNode>&             inDomainNode,
                          const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inSourceArgumentNodes,
                          const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inDestinationArgumentNodes, HTNEnvironment& inSourceEnvironment,
                          HTNEnvironment& inDestinationEnvironment, const std::vector<std::string>& inSourcePrefixes,
                          const std::vector<std::string>& inDestinationPrefixes);

bool CopyArgumentsConst(const std::shared_ptr<const HTNDomainNode>&             inDomainNode,
                        const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inSourceArgumentNodes,
                        const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inDestinationArgumentNodes,
                        const HTNEnvironment& inSourceEnvironment,
                        HTNEnvironment& inDestinationEnvironment, const std::vector<std::string>& inSourcePrefixes,
                        const std::vector<std::string>& inDestinationPrefixes);

bool RemovePrefix(const std::string& inPrefix, std::string& inString);

inline const std::string              InputPrefix       = "inp_";
inline const std::string              OutputPrefix      = "out_";
inline const std::string              InputOutputPrefix = "io_";
inline const std::vector<std::string> InputPrefixes     = {InputPrefix, InputOutputPrefix};
inline const std::vector<std::string> OutputPrefixes    = {OutputPrefix, InputOutputPrefix};
} // namespace HTN::Helpers
