#pragma once

class HTNDatabaseHook;
class HTNPlannerHook;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNDatabaseHook& inDatabaseHook);

    void Render(bool& _IsOpen);

private:
    void RenderPlan();
    void RenderDecomposition();
    void RenderDomain();
    void RenderWorldState();

    HTNPlannerHook*  mPlannerHook  = nullptr;
    HTNDatabaseHook* mDatabaseHook = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNDatabaseHook& inDatabaseHook)
    : mPlannerHook(&inPlannerHook), mDatabaseHook(&inDatabaseHook)
{
}
