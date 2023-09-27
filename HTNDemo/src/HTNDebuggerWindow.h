#pragma once

class HTNPlannerHook;
class HTNWorldStateHook;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNWorldStateHook& inWorldStateHook);

    void Render(bool& _IsOpen);

private:
    void RenderActivePlan();
    void RenderDecomposition();
    void RenderParsing();
    void RenderDatabase();

    HTNPlannerHook*    mPlannerHook    = nullptr;
    HTNWorldStateHook* mWorldStateHook = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNWorldStateHook& inWorldStateHook)
    : mPlannerHook(&inPlannerHook), mWorldStateHook(&inWorldStateHook)
{
}
