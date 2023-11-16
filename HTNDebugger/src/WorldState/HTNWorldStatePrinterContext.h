#pragma once

#ifdef HTN_DEBUG
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
