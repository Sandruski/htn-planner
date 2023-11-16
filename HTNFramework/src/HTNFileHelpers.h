#pragma once

#include <filesystem>
#include <string>

namespace HTNFileHelpers
{
std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath);
} // namespace HTNFileHelpers

namespace HTNFileHelpers
{
std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath)
{
    return (std::filesystem::current_path().parent_path() / std::filesystem::path(inRelativePath));
}
} // namespace HTNFileHelpers
