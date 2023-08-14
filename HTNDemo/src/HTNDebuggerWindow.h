#pragma once

class HTNPlannerHook;
class HTNWorldState;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlanner, HTNWorldState& inWorldState);

    void Render(bool& _IsOpen);

private:
    void RenderPlan();
    void RenderDatabase();
    void RenderDecomposition();

    HTNPlannerHook* mPlanner    = nullptr;
    HTNWorldState*  mWorldState = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlanner, HTNWorldState& inWorldState) : mPlanner(&inPlanner), mWorldState(&inWorldState)
{
}