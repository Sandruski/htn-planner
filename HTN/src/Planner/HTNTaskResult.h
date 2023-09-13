#pragma once

#include "HTNAtom.h"

#include <string>
#include <vector>

class HTNTaskResult
{
public:
    explicit HTNTaskResult(const std::string& inName, const std::vector<HTNAtom>& inArguments);

    // TODO salvarez Remove name and make 1 argument be the name
    const std::string&          GetName() const;
    const std::vector<HTNAtom>& GetArguments() const;

private:
    std::string          mName;
    std::vector<HTNAtom> mArguments;
};

inline HTNTaskResult::HTNTaskResult(const std::string& inName, const std::vector<HTNAtom>& inArguments) : mName(inName), mArguments(inArguments)
{
}

inline const std::string& HTNTaskResult::GetName() const
{
    return mName;
}

inline const std::vector<HTNAtom>& HTNTaskResult::GetArguments() const
{
    return mArguments;
}
