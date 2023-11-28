// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"
#include "HTNCoreMinimal.h"
#include "WorldState/HTNWorldStateFwd.h"

struct ImGuiTextFilter;

/**
 * Contextual data of a world state printer
 */
class HTNWorldStatePrinterContext final : public HTNNodeVisitorContextBase
{
public:
    explicit HTNWorldStatePrinterContext(const HTNWorldState& inWorldState, const ImGuiTextFilter& inTextFilter);

    // Returns the world state
    HTN_NODISCARD const HTNWorldState& GetWorldState() const;

    // Returns the text filter
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
