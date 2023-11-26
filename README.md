# HTN
This project is a hierarchical task network (HTN) planner library for AI decision-making. It is implemented in C++ and it interprets a custom domain language.

[![Video](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_successful_choice_point_tab.png)](https://github.com/Sandruski/htn-planner/tree/main/docs/videos/test_video.mp4)

## Disclaimer
Note that the project is a work in progress (WIP). As such, certain features may be incomplete or subject to change. Feedback and contributions are welcomed.

## Getting Started

### Use pre-built binaries
1. Download the latest release from [Releases](https://github.com/Sandruski/htn-planner/releases). The latest released version is v0.1.
2. Run `HTNDemo.exe`, which corresponds to the `HTNDemo` project.

### Build it yourself
1. Clone the repository with `git clone https://github.com/Sandruski/htn-planner` or download it as a ZIP from [htn-planner](https://github.com/Sandruski/htn-planner).
2. Generate the solution by running [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat). By default, it is generated for Visual Studio 2022. You can change the version of Visual Studio by editing this file.
3. If you add or remove project files, regenerate the solution by rerunning [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat).
4. Build the solution and run the startup project. The startup project should be configured to either `HTNDemo`, as is the default setting, or `HTNTest`.

## Quick Introduction
The solution contains four projects, `HTNFramework`, `HTNDebugger`, `HTNDemo`, and `HTNTest`. `HTNFramework` and `HTNDebugger` are libraries, `HTNFramework` is the planner itself and `HTNDebugger` is the debugger of the planner. `HTNDemo` and `HTNTest` are applications, `HTNDemo` is a playground for using the planner and `HTNTest` contains a set of unit tests for testing the planner using automation.

### HTNDemo
1. In the `World State` tab, click the `Parse` button to parse the selected world state file from the folder of the same name. Below, see the parsed world state.

![world_state](https://github.com/Sandruski/htn-planner/blob/main/docs/images/world_state_tab.png)

2. In the `Domain` tab, click the `Parse` button to parse the selected domain file from the folder of the same name. Below, see the parsed domain.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/domain_tab.png)

3. In the `Decomposition` tab, click the `Decompose` button to decompose the selected entry point of the parsed domain using the parsed world state. Below, see the result of the decomposition.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_successful_choice_point_tab.png)
![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_choice_points_tab.png)
![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_failed_choice_point_tab.png)
![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition_tab.png)

// TODO salvarez explain
- Uses DFS.
- Uses backtracking to test alternative options for choice points.

4. In the `Active Plan` tab, see the decomposed plan.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_active_plan_tab.png)
![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_active_plan_tab.png)

### HTNTest
1. In the console window, see the results of the tests.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/test_result_window.png)

## Performance Analysis

// TODO salvarez explain and take screenshots from Optick

## Third-Party
- [SDL](https://www.libsdl.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GoogleTest](https://google.github.io/googletest/)
- [Premake](https://premake.github.io/)
- [Optick](https://github.com/bombomby/optick)

## Reference
- [Exploring HTN Planners
through Example](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)
- [derplanner](https://github.com/alexshafranov/derplanner)
- [The AI of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-ai-of-horizon-zero-dawn)
- [Hierarchical AI for Multiplayer
Bots in Killzone 3](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter29_Hierarchical_AI_for_Multiplayer_Bots_in_Killzone_3.pdf)

## Future Work
- Implement call terms for invoking functions defined in C++.
- Implement includes of additional domain files for improved modularity.
- Implement overrides of methods, axioms, and constants for improved customization.
- Edit and export domains and world states in a custom editor with syntax highlighting for faster iterations.
- Implement a domain validator to ensure that the parsed domain has semantic coherence.
- Time-slice the decomposition process to maintain a stable framerate.
- Transition from an interpreter to a compiler to improve the overall performance.

## License
This project is under the [MIT license](https://github.com/Sandruski/htn-planner/blob/main/LICENSE).