#pragma once

#include "HTNAtom.h"

#include <string>
#include <vector>

class HTNTaskResult
{
public:
    explicit HTNTaskResult(const std::string& inID, const std::vector<HTNAtom>& inArguments);

    const std::string&          GetID() const;
    const std::vector<HTNAtom>& GetArguments() const;

private:
    std::string          mID;
    std::vector<HTNAtom> mArguments;
};

inline HTNTaskResult::HTNTaskResult(const std::string& inID, const std::vector<HTNAtom>& inArguments) : mID(inID), mArguments(inArguments)
{
}

inline const std::string& HTNTaskResult::GetID() const
{
    return mID;
}

inline const std::vector<HTNAtom>& HTNTaskResult::GetArguments() const
{
    return mArguments;
}
