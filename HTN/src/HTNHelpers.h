#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionContext;
class HTNValueNode;

namespace HTN::Helpers
{
bool CopyArguments(HTNDecompositionContext& ioDecompositionContext, const std::vector<std::shared_ptr<const HTNValueNode>>& inSourceArgumentNodes,
                   const std::vector<std::shared_ptr<const HTNValueNode>>& inDestinationArgumentNodes, const std::string& inSourceScopeID,
                   const std::string& inDestinationScopeID, const std::vector<std::string>& inSourcePrefixes,
                   const std::vector<std::string>& inDestinationPrefixes);

bool RemovePrefix(const std::string& inPrefix, std::string& inString);

inline const std::string              InputPrefix       = "inp_";
inline const std::string              OutputPrefix      = "out_";
inline const std::string              InputOutputPrefix = "io_";
inline const std::vector<std::string> InputPrefixes     = {InputPrefix, InputOutputPrefix};
inline const std::vector<std::string> OutputPrefixes    = {OutputPrefix, InputOutputPrefix};
} // namespace HTN::Helpers