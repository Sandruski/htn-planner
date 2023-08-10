#pragma once

class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlanningUnit& inPlanningUnit);

    void Render(bool& _IsOpen);

private:
    void RenderActivePlan();
    void RenderDatabase();
    void RenderDecomposition();

    HTNPlanningUnit* mPlanningUnit = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlanningUnit& inPlanningUnit)
    : mPlanningUnit(&inPlanningUnit)
{
}
