---
id: task-048-01-last-level-profile
description: "Current wave director values saved as a last-level profile in the data, not wired."
parent: task-048-last-level-profile
kind: task
status: in-progress
---
# Last-level director profile

- goal: Current wave director values saved as a last-level profile in the data, not wired.
- agent: cpp
- depends_on: none
- inputs: difficulty (vault/reference/gdd/run/difficulty.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: a second named config FRRWaveDirectorConfig::MakeLastLevel() (or equivalent data asset in C++) holding exactly the current MakeDefault values (150 alive, waves 7.1 → 4.1 s, distinct-side packs, current packs and hordes), with a comment that it is not used until levels exist; MakeDefault untouched by this task; no test; docs sys-spawner-director.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-048-last-level-profile]] — parent global task.
