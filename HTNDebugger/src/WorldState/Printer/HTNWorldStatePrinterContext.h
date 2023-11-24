// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"

class HTNWorldState;

struct ImGuiTextFilter;

class HTNWorldStatePrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNWorldStatePrinterContext(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter);

    HTN_NODISCARD const HTNWorldState&   GetWorldState() const;
    HTN_NODISCARD const ImGuiTextFilter& GetTextFilter() const;

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
