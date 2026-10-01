---
id: task-004-01-enable-gas
description: "Enable the GameplayAbilities plugin and the GAS modules in the build, as specified by task-000, and rebuild."
parent: task-004-miner-gas
kind: task
status: done
---
# Enable GAS

- goal: the project compiles with GAS available.
- agent: cpp
- depends_on: none
- inputs: [[task-000-enable-gas]] (the spec of this work), [[build-toolchain]], [[gas-engine-facts]].
- expected_output: `GameplayAbilities` plugin enabled in `RogueRigs.uproject`; `GameplayAbilities`, `GameplayTags`, `GameplayTasks` in the module dependencies of `RogueRigs.Build.cs` (changes to these two files allowed for this only); [[build-toolchain]] no longer says GAS must be enabled.
- done_when: command-line build exits 0 with the editor closed; lint clean.
- editor_state: closed
- token_ceiling: 20000
- milestone: M1
- result: GameplayAbilities enabled in the .uproject; GameplayAbilities, GameplayTags, GameplayTasks public dependencies; [[build-toolchain]] updated; build exit 0 (orchestrator re-ran it). Closes [[task-000-enable-gas]]. About 23k tokens.

## Relations
- [[task-004-miner-gas]] — parent global task.
