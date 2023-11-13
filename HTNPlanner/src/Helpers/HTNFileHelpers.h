#pragma once

#include <filesystem>
#include <string>

namespace HTNFileHelpers
{
std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath);

const std::string WorldStatesDirectoryName = "WorldStates";
const std::string WorldStateFileExtension  = ".worldstate";
const std::string DomainsDirectoryName     = "Domains";
const std::string DomainFileExtension      = ".domain";
} // namespace HTNFileHelpers

namespace HTNFileHelpers
{
std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath)
{
    return (std::filesystem::current_path().parent_path() / std::filesystem::path(inRelativePath));
}
} // namespace HTNFileHelpers
