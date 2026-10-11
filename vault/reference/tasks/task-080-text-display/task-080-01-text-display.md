---
id: task-080-01-text-display
description: "A4 display: upgrade card flavor lines and Kessler-Voss alerts are shown in game; bark subtitle only if time allows."
parent: task-080-text-display
kind: task
status: done
---
# A4 text display

- goal: A4 display: upgrade card flavor lines and Kessler-Voss alerts are shown in game; bark subtitle only if time allows.
- agent: bp
- depends_on: task-079-01-text-loader
- inputs: assignment-4-plan (vault/reference/course/assignments/assignment-4-plan.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-level-up (vault/reference/tech/systems/sys-level-up.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: in the existing card and alert widgets, show the flavor line under the stat name and the alert text from the loader through events calling C++; thin Blueprint layer only; every changed Blueprint documented in tech/blueprints. Group the work and commit after each group (bp 40-turn limit). Bark subtitle only on the owner order after the three contents are validated. No screenshot capture calls.
- done_when: cards and alerts show the loaded text in PIE (property route, no screenshot); Blueprint doc nodes updated; lint clean.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Validated by the owner after play tests (2026-10-11); weapon info and horde text bugs filed as tasks 098 and 099.

## Relations
- [[task-080-text-display]] — parent global task.
