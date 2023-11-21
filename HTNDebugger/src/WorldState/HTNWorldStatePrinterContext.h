// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

class HTNWorldState;

struct ImGuiTextFilter;

class HTNWorldStatePrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNWorldStatePrinterContext(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter);

    const HTNWorldState&   GetWorldState() const;
    const ImGuiTextFilter& GetTextFilter() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const HTNWorldState&   mWorldState;
    const ImGuiTextFilter& mTextFilter;
};

inline const HTNWorldState& HTNWorldStatePrinterContext::GetWorldState() const
{
    return mWorldState;
}

inline const ImGuiTextFilter& HTNWorldStatePrinterContext::GetTextFilter() const
{
    return mTextFilter;
}
#endif
