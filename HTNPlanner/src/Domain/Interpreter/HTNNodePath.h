#pragma once

#include "HTNCoreMinimal.h"

#include <string>

/**
 * Path to a node in a domain
 */
class HTNNodePath
{
public:
    bool               TryPushNodeToNodePath(const std::string& inNodeID);
    bool               TryPopNodeFromNodePath();
    const std::string& GetNodePath() const;

private:
    std::string mNodePath;
};

inline const std::string& HTNNodePath::GetNodePath() const
{
    return mNodePath;
}
