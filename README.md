# HTN
This project is a hierarchical task network (HTN) planner library for AI decision-making. It is implemented in C++ and it interprets a custom domain language.

## Disclaimer
Note that the project is a work in progress (WIP). As such, certain features may be incomplete or subject to change. Feedback and contributions are welcomed.

## Getting Started
1. Clone the repository with `git clone https://github.com/Sandruski/htn-planner`.
2. Generate the solution by running [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat). By default, it is generated for Visual Studio 2022. You can change the version of Visual Studio by editing this file.
3. If you add or remove project files, regenerate the solution by rerunning [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat).

## Quick Introduction
The solution contains five projects, HTNPlanner, HTNDebugger, HTNDemo, HTNTest, and HTNFramework. HTNPlanner is the planner library, HTNDebugger is a library to debug the planner, HTNDemo is a playground for using the planner, HTNTest contains a set of unit tests for testing the planner using automation, and HTNFramework contains the core functionalities shared across all the projects. The startup project should be configured to either HTNPlanner, as is the default setting, or HTNTest.

## Performance Analysis

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
- Implement call terms (functions defined in C++).
- Allow to include other domain files.
- Allow to override methods, axioms, and constants.
- Allow to edit and export domains and world states in a custom editor with syntax highlighting.
- Implement a domain validator.
- Time-slice the decomposition process.
- Replace the interpreter with a compiler.

## License
This project is under the [MIT license](https://github.com/Sandruski/htn-planner/blob/main/LICENSE).