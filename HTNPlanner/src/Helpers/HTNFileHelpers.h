#pragma once

#include <filesystem>
#include <string>

namespace HTNFileHelpers
{
std::string MakeAbsolutePath(const std::string& inRelativePath);
} // namespace HTNFileHelpers

namespace HTNFileHelpers
{
std::string MakeAbsolutePath(const std::string& inRelativePath)
{
    return (std::filesystem::current_path().parent_path() / std::filesystem::path(inRelativePath)).string();
}
} // namespace HTNFileHelpers
