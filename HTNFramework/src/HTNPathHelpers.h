#pragma once

#include <filesystem>
#include <string>

namespace HTNPathHelpers
{
bool TryPushSegmentToPath(const std::string& inSegment, std::string& ioPath);
bool TryPopSegmentFromPath(std::string& ioPath);
bool FindLastPathSegment(const std::string& inPath, std::string& outSegment);

std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath);
} // namespace HTNPathHelpers

namespace HTNPathHelpers
{
inline std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath)
{
    const std::filesystem::path ParentPath = std::filesystem::current_path().parent_path();
    return (ParentPath / std::filesystem::path(inRelativePath));
}
} // namespace HTNPathHelpers
