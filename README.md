# HTN
This project is a hierarchical task network (HTN) planner library for AI decision-making. It is implemented in C++ and it interprets a custom domain language.

Not being just a planner but a complete langauge instead makes it a powerful tool for content creators to script complex behaviors with simple statements.

![main_decomposition](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition.png)
*Main decomposition*

**Watch the [demo](https://github.com/Sandruski/htn-planner/tree/main/docs/videos/demo.mp4) video.**

## Disclaimer
Note that the project is a work in progress (WIP). As such, certain features may be incomplete or subject to change. Feedback and contributions are welcomed.

## Getting Started

### Use pre-built binaries
1. Download the latest release from [Releases](https://github.com/Sandruski/htn-planner/releases). The latest released version is [v0.1](https://github.com/Sandruski/htn-planner/releases/tag/v0.1).
2. Run `bin/HTNDemo.exe`, which corresponds to the `HTNDemo` project.

### Build it yourself
1. Clone the repository with `git clone https://github.com/Sandruski/htn-planner` or download it as a ZIP from [htn-planner](https://github.com/Sandruski/htn-planner).
2. Generate the solution by running [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat). By default, it is generated for Visual Studio 2022. You can change the version of Visual Studio by editing this file.
3. If you add or remove project files, regenerate the solution by rerunning [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat).
4. Build the solution and run the startup project. The startup project should be configured to either `HTNDemo`, as is the default setting, or `HTNTest`.

## Quick Introduction
The solution contains four projects, `HTNFramework`, `HTNDebugger`, `HTNDemo`, and `HTNTest`. `HTNFramework` and `HTNDebugger` are libraries, `HTNFramework` is the planner itself and `HTNDebugger` is the debugger of the planner. `HTNDemo` and `HTNTest` are applications, `HTNDemo` is a playground for using the planner and `HTNTest` contains a set of unit tests for testing the planner using automation.

### HTNDemo
1. In the `World State` tab, click the `Parse` button to parse the selected world state file. World state files are identified by the `.worldstate` extension and are located in the folder of the same name.

A world state is a database of tables of facts. It represents the knowledge about the world.

![world_state](https://github.com/Sandruski/htn-planner/blob/main/docs/images/world_state.png)
*World state*

2. In the `Domain` tab, click the `Parse` button to parse the selected domain file. Domain files are identified by the `.domain` extension and are located in the folder of the same name.

A domain is a graph of constants, axioms, and methods. It represents the available actions.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/domain.png)
*Domain*

3. In the `Decomposition` tab, click the `Decompose` button to decompose the selected entry point of the parsed domain using the parsed world state. Entry points are methods tagged with `top_level_method` of a domain tagged with `top_level_domain`.

The decomposition performs a depth-first search (DFS) on the domain graph, starting at the top level compound task and expanding hierarchically by exploring compound tasks. The algorithm uses backtracking as a mechanism to try alternative options leading to different paths at choice points.

![main_decomposition_successful_choice_point](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_successful_choice_point.png)
*Successful choice point*

![main_decomposition_choice_points](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_choice_points.png)
*Choice points*

![main_decomposition_failed_choice_point](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_failed_choice_point.png)
*Failed choice point*

![secondary_decomposition](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition.png)
*Secondary decomposition*

4. In the `Active Plan` tab, see the tasks of the active plan resulting from the decomposition.

A plan is a sequence of primitive tasks, which consist of an identifier and a list of arguments.

![main_active_plan](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_active_plan.png)
*Main active plan*

![secondary_active_plan](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_active_plan.png)
*Secondary active plan*

### HTNTest
1. In the console window, see the results of the unit tests.

![unit_tests_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/unit_tests_results.png)
*Unit tests results*

## Performance Analysis

Since the purpose of this initial iteration of the project has been to understand the problem domain, the focus has been on functionality rather than performance. Because of this, the current implementation of the planner uses an interpreter for the custom domain language. However, the plan is to transition to a compiler in the future, which will improve the overall performance among other things.

The following are the profiling results obtained during a session for the `HTNDemo` project in Release configuration with both `HTN_VALIDATE_DOMAIN`, which enables validating the domain during a decomposition, and `HTN_DEBUG_DECOMPOSITION`, which enables storing the decomposition results during a decomposition, macros defined. Both the function responsible for decomposing a domain, named `InterpretDomain`, and displaying it on the debug window, named `PrintDecomposition`, are mesured, because they are expected to be the ones that are called more frequently. Additional code has also been instrumented.

![main_decomposition_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_profiling_results.png)
*Main decomposition profiling results*

On average, a decomposing the main planning unit takes 0.4ms and displaying the results takes 0.15ms.

![secondary_decomposition_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition_profiling_results.png)
*Secondary decomposition profiling results*

On average, a decomposing the secondary planning unit takes 0.03ms and displaying the results takes 0.02ms.

Furthermore, a profiling session is started each time the `HTNTest` project is run, and the resulting capture is saved in the `Captures` directory.

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