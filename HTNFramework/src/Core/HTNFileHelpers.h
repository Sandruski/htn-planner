// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <filesystem>
#include <string>

namespace HTNFileHelpers
{
void CollectFilePaths(const std::filesystem::path& inDirectoryPath, const std::string& inFileExtension,
                                 std::vector<std::filesystem::path>& outFilePaths);

void CollectFilePathsRecursively(const std::filesystem::path& inDirectoryPath, const std::string& inFileExtension,
                                 std::vector<std::filesystem::path>& outFilePaths);

HTN_NODISCARD std::string           MakeFilePath(const std::string& inDirectoryName, const std::string& inFileName, const std::string& inFileExtension);
HTN_NODISCARD std::string           MakeFilePathDisplayName(const std::filesystem::path& inFilePath, const std::filesystem::path& inDirectoryPath);
HTN_NODISCARD std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath);

inline const std::string kWorldStatesDirectoryName = "WorldStates";
inline const std::string kDomainsDirectoryName     = "Domains";
inline const std::string kTestDirectoryName        = "Test";
inline const std::string kCapturesDirectoryName    = "Captures";
inline const std::string kCaptureFileName          = "test";
inline const std::string kWorldStateFileExtension  = ".worldstate";
inline const std::string kDomainFileExtension      = ".domain";
} // namespace HTNFileHelpers

namespace HTNFileHelpers
{
inline std::filesystem::path MakeAbsolutePath(const std::string& inRelativePath)
{
    const std::filesystem::path ParentPath = std::filesystem::current_path().parent_path();
    return ParentPath / std::filesystem::path(inRelativePath);
}
} // namespace HTNFileHelpers
