#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNValue;
class HTNDecompositionRecord;

namespace HTN::Helpers
{
bool CopyArguments(HTNDecompositionRecord& inDecomposition, const std::vector<std::shared_ptr<const HTNValue>>& inSourceArguments,
                   const std::vector<std::shared_ptr<const HTNValue>>& inDestinationArguments, const std::string& inSourceScopeID,
                   const std::string& inDestinationScopeID, const std::vector<std::string>& inSourcePrefixes,
                   const std::vector<std::string>& inDestinationPrefixes);

bool RemovePrefix(const std::string& inPrefix, std::string& inString);

inline const std::string              InputPrefix       = "inp_";
inline const std::string              OutputPrefix      = "out_";
inline const std::string              InputOutputPrefix = "io_";
inline const std::vector<std::string> InputPrefixes     = {InputPrefix, InputOutputPrefix};
inline const std::vector<std::string> OutputPrefixes    = {OutputPrefix, InputOutputPrefix};
} // namespace HTN::Helpers
