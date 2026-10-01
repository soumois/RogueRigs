---
id: task-000-enable-gas
description: "Enable the GameplayAbilities plugin and add the GAS modules to the build, so GAS code can compile."
parent: tasks-index
kind: task
status: done
---
# Enable GAS in the project

- goal: the project compiles with GAS available. The plugin is off by default (`EnabledByDefault: false`) and not listed in `RogueRigs.uproject`; `RogueRigs.Build.cs` does not depend on its modules yet.
- agent: orchestrator (task 0), executed by the owner or the C++ agent.
- inputs: `RogueRigs.uproject`, `Source/RogueRigs/RogueRigs.Build.cs`, [[sys-gas-core]].
- expected_output: plugin `GameplayAbilities` enabled in the `.uproject`; `GameplayAbilities`, `GameplayTags`, `GameplayTasks` added to the Build.cs dependencies; clean rebuild with the editor closed (about 10 min).
- token_ceiling: open
- milestone: M1
- result: executed as [[task-004-01-enable-gas]] (2026-10-01): plugin enabled, three modules public in Build.cs, build exit 0.

## Relations
- [[tasks-index]] — parent index.
