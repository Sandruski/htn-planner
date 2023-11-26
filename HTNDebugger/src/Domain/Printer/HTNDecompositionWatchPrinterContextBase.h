// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>
#include <vector>

class HTNNodeInstance;
class HTNDomainNode;

class HTNDecompositionWatchPrinterContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionWatchPrinterContextBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNNodeInstance& inNodeInstance);
    virtual ~HTNDecompositionWatchPrinterContextBase() = 0;

    void AddVariablePath(const std::string& inVariablePath);

    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    HTN_NODISCARD const HTNNodeInstance&                      GetNodeInstance() const;
    HTN_NODISCARD const std::vector<std::string>& GetNodeVariablePaths() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const HTNNodeInstance&                      mNodeInstance;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    std::vector<std::string> mNodeVariablePaths;
};

inline void HTNDecompositionWatchPrinterContextBase::AddVariablePath(const std::string& inVariablePath)
{
    mNodeVariablePaths.emplace_back(inVariablePath);
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionWatchPrinterContextBase::GetDomainNode() const
{
    return mDomainNode;
}

inline const HTNNodeInstance& HTNDecompositionWatchPrinterContextBase::GetNodeInstance() const
{
    return mNodeInstance;
}

inline const std::vector<std::string>& HTNDecompositionWatchPrinterContextBase::GetNodeVariablePaths() const
{
    return mNodeVariablePaths;
}
#endif
