# HTN
This project is a hierarchical task network (HTN) planner library for AI decision-making. It is implemented in C++ and interprets a custom domain language.

Not being just a planner but a complete language instead makes it a powerful tool for content creators to script complex behaviors using simple statements.

```
(:domain Example top_level_domain

    (:method (main) top_level_method

        (branch_hello_world
            (
            )
            (       (!log "Hello world!")
            )
        )
    )
)
```
*Example domain syntax*

![example_decomposition](https://github.com/Sandruski/htn-planner/blob/main/docs/images/example_decomposition.png "Example decomposition")
*Example decomposition*

## Disclaimer
Note that the project is a work in progress (WIP). As such, certain features may be incomplete or subject to change. Feedback and contributions are welcomed.

## Getting Started

### Use pre-built binaries
1. Download the latest release from [Releases](https://github.com/Sandruski/htn-planner/releases). The latest released version is [v0.1](https://github.com/Sandruski/htn-planner/releases/tag/v0.1).
2. Run `bin/HTNDemo.exe`, which corresponds to the `HTNDemo` project.

### Build it yourself
1. Clone the repository with `git clone https://github.com/Sandruski/htn-planner` or download it as a ZIP from [htn-planner](https://github.com/Sandruski/htn-planner).
2. Generate the solution by running [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat). By default, it is generated for Visual Studio 2022. You can change the version of Visual Studio by editing this file.
3. If you add or remove project files or modify `premake5.lua`, regenerate the solution by rerunning [GenerateProjectFiles.bat](https://github.com/Sandruski/htn-planner/blob/main/GenerateProjectFiles.bat).
4. Build the solution and run the startup project. The startup project should be configured to either `HTNDemo`, as is the default setting, or `HTNTest`.

## Quick Introduction
The solution contains four projects, `HTNFramework`, `HTNDebugger`, `HTNDemo`, and `HTNTest`. `HTNFramework` and `HTNDebugger` are libraries, being `HTNFramework` the planner itself and `HTNDebugger` the debugger of the planner. `HTNDemo` and `HTNTest` are applications, being `HTNDemo` a playground for using the planner and `HTNTest` a set of unit tests for testing the planner.

### HTNDemo
**Watch the [demo](https://github.com/Sandruski/htn-planner/tree/main/docs/videos/demo.mp4) video.**

1. In the `World State` tab, click the `Parse` button to parse the selected world state file. World state files are identified by the `.worldstate` extension and are located in the folder of the same name.

A world state is a database of facts that represent the knowledge about the world.

The `HTNWorldStateLexer` class is responsible for lexing the text of a world state file and the `HTNWorldStateParser` class is responsible for parsing the resulting tokens into an object of the `HTNWorldState` class, which is the actual world state.

![world_state](https://github.com/Sandruski/htn-planner/blob/main/docs/images/world_state.png "World state")
*World state*

2. In the `Domain` tab, click the `Parse` button to parse the selected domain file. Domain files are identified by the `.domain` extension and are located in the folder of the same name.

A domain is a graph of constants, axioms, and methods that represent the available actions.

The `HTNDomainLexer` class is responsible for lexing the text of a domain file and the `HTNDomainParser` class is responsible for parsing the resulting tokens into an object of the `HTNDomainNode` class, which is the actual domain.

![domain](https://github.com/Sandruski/htn-planner/blob/main/docs/images/domain.png "Domain")
*Domain*

3. In the `Decomposition` tab, click the `Decompose` button to decompose the selected entry point of the parsed domain using the parsed world state. Entry points are methods tagged with `top_level_method` of a domain tagged with `top_level_domain`.

The decomposition process performs a depth-first search (DFS) on the domain graph. The algorithm starts at the top-level compound task and hierarchically expands it into a sequence of primitive tasks, which represent a plan. It uses a backtracking mechanism to try different options at choice points, which lead to different paths. It is architected to be executed in parallel and take advantage of multithreading.

The `HTNDomainInterpreter` class is responsible for the decomposition process. The `HTNDecompositionPrinter` class is responsible for displaying its results on the debug window.

![main_decomposition_choice_point_successful_option](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_choice_point_successful_option.png "Main decomposition choice point successful option")
*Main decomposition choice point successful option. The main decomposition found the successful path at the 5th try*

![main_decomposition_choice_points](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_choice_point_options.png "Main decomposition choice point options")
*Main decomposition choice point options. The main decomposition tried 5 different options at its only choice point, meaning that it backtracked 4 times*

![main_decomposition_choice_point_failed_option](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_choice_point_failed_option.png "Main decomposition choice point failed option")
*Main decomposition choice point failed option. The main decomposition found a failed path at the first try*

![secondary_decomposition](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition.png "Secondary decomposition")
*Secondary decomposition. The secondary decomposition found the successful path at the first try, therefore it did not try different options at its only choice point, meaning that it did not backtrack*

4. In the `Active Plan` tab, see the tasks of the active plan resulting from the decomposition.

A plan is a sequence of primitive tasks, which consist of an identifier and a list of arguments. The primitive tasks themselves lack inherent meaning, relying on the user to interpret them according to their needs.

![main_active_plan](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_active_plan.png "Main active plan")
*Main active plan*

![secondary_active_plan](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_active_plan.png "Secondary active plan")
*Secondary active plan*

### HTNTest
1. In the console window, see the results of the unit tests.

Currently, there is a single unit test that decomposes a specified entry point of a specified domain file using a specified a world state file. However, the plan is to expand the test suite with additional tests that cover the individual functionalities of the systems.

![unit_tests_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/unit_tests_results.png "Unit tests results")
*Unit tests results*

## Performance Analysis

Since the purpose of this initial iteration of the project has been to understand the problem domain, the focus has been on functionality rather than performance. For this reason, the current implementation of the planner uses an interpreter for the custom domain language and has no optimizations. However, the plan is to transition to a compiler, which will improve the overall performance.

The following are the profiling results obtained on an AMD Ryzen 9 5900X 12-Core Processor with 64GB of RAM. The two functions measured are the one responsible for decomposing a domain, named `InterpretDomain`, and the one responsible for displaying it on the debug window, named `PrintDecomposition`, because they are expected to be the ones that are called more frequently. The `InterpretDomain` function is measured without any custom macros defined and the `PrintDecomposition` function is measured with the `HTN_DEBUG_DECOMPOSITION` macro defined, which enables storing the state of each step of a decomposition for debug purposes.

![main_decomposition_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_profiling_results.png "Main decomposition profiling results")
*Main decomposition profiling results. On average, performing the main decomposition takes 0.1ms*

![main_decomposition_printer_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/main_decomposition_printer_profiling_results.png "Main decomposition printer profiling results")
*Main decomposition printer profiling results. On average, displaying the main decomposition takes 0.11ms*

![secondary_decomposition_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition_profiling_results.png "Secondary decomposition profiling results")
*Secondary decomposition profiling results. On average, performing the secondary decomposition takes 0.01ms*

![secondary_decomposition_printer_profiling_results](https://github.com/Sandruski/htn-planner/blob/main/docs/images/secondary_decomposition_printer_profiling_results.png "Secondary decomposition printer profiling results")
*Secondary decomposition printer profiling results. On average, displaying the secondary decomposition takes 0.025ms*

Furthermore, a profiling session is started each time the `HTNTest` project is run, and the resulting capture is saved in the `Captures` directory.

## Third-Party
- [SDL](https://www.libsdl.org)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GoogleTest](https://google.github.io/googletest)
- [Premake](https://premake.github.io)
- [Optick](https://github.com/bombomby/optick)

## Reference
- [Exploring HTN Planners
through Example](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)
- [derplanner](https://github.com/alexshafranov/derplanner)
- [The AI of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-ai-of-horizon-zero-dawn)
- [Hierarchical AI for Multiplayer
Bots in Killzone 3](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter29_Hierarchical_AI_for_Multiplayer_Bots_in_Killzone_3.pdf)
- [Crafting Interpreters](https://craftinginterpreters.com)

## Future Work
- Implement call terms for invoking functions defined in C++.
- Implement includes of additional domain files for improved modularity.
- Implement overrides of methods, axioms, and constants for improved customization.
- Edit and export domains and world states in a custom editor with syntax highlighting for faster iterations.
- Implement a domain validator to ensure that the parsed domain has semantic coherence.
- Time-slice the decomposition process to maintain a stable framerate.
- Transition from an interpreter to a compiler to improve the overall performance.
- Add additional unit tests that cover the individual functionalitites of the systems.

## License
This project is under the [MIT license](https://github.com/Sandruski/htn-planner/blob/main/LICENSE).