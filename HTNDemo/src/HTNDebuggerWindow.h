#pragma once

class HTNPlannerHook;
class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlanner, HTNPlanningUnit& inPlanningUnit);

    void Render(bool& _IsOpen);

private:
    void RenderPlan();
    void RenderDatabase();
    void RenderDecomposition();

    HTNPlannerHook*  mPlanner      = nullptr;
    HTNPlanningUnit* mPlanningUnit = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlanner, HTNPlanningUnit& inPlanningUnit)
    : mPlanner(&inPlanner), mPlanningUnit(&inPlanningUnit)
{
}
