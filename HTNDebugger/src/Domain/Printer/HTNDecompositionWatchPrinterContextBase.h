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

/**
 * Base class for the contextual data of a decomposition watch printer
 */
class HTNDecompositionWatchPrinterContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionWatchPrinterContextBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNNodeInstance& inNodeInstance);
    virtual ~HTNDecompositionWatchPrinterContextBase() = 0;

    // Adds a new variable path
    void AddVariablePath(const std::string& inVariablePath);

    // Returns the domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

    // Returns the node instance
    HTN_NODISCARD const HTNNodeInstance& GetNodeInstance() const;

    // Returns the paths of the variables
    HTN_NODISCARD const std::vector<std::string>& GetVariablePaths() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const HTNNodeInstance&                      mNodeInstance;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    std::vector<std::string> mVariablePaths;
};

inline void HTNDecompositionWatchPrinterContextBase::AddVariablePath(const std::string& inVariablePath)
{
    mVariablePaths.emplace_back(inVariablePath);
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionWatchPrinterContextBase::GetDomainNode() const
{
    return mDomainNode;
}

inline const HTNNodeInstance& HTNDecompositionWatchPrinterContextBase::GetNodeInstance() const
{
    return mNodeInstance;
}

inline const std::vector<std::string>& HTNDecompositionWatchPrinterContextBase::GetVariablePaths() const
{
    return mVariablePaths;
}
#endif
