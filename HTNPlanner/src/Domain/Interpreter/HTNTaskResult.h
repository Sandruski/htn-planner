#pragma once

#include "Helpers/HTNAtom.h"

#include <string>
#include <vector>

class HTNTaskResult
{
public:
    explicit HTNTaskResult(const std::string& inID, const std::vector<HTNAtom>& inArguments);

    const std::string&          GetID() const;
    const std::vector<HTNAtom>& GetArguments() const;

private:
    // ID of the task
    std::string mID;

    // Arguments of the task
    std::vector<HTNAtom> mArguments;
};

inline const std::string& HTNTaskResult::GetID() const
{
    return mID;
}

inline const std::vector<HTNAtom>& HTNTaskResult::GetArguments() const
{
    return mArguments;
}
