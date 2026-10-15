---
id: task-182-01-debris-lifetime-4-5s
description: "Debris stay 4.5 s at full size, then sink in 0.4 s: lifetime 4.9 s, sink start moved from 3.0 s to 4.5 s."
parent: task-182-debris-pool-80-test
kind: task
status: done
---
# Debris lifetime 4.5 s

- goal: Debris stay 4.5 s at full size, then sink in 0.4 s: lifetime 4.9 s, sink start moved from 3.0 s to 4.5 s (owner 2026-10-14).
- agent: bp
- depends_on: none
- inputs: bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: NS_RobotDebris: InitializeParticle lifetime 4.9; the age threshold 3.0 of the collision plane gate and of the sink velocity module moved to 4.5 (both expressions); still one-shot; bp-robot-debris updated
- done_when: Lifetime and both thresholds read back; compiles with no stack issue; saved by explicit path; doc updated; committed by path; editor stays open
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 36, cache write 38670, cache read 481040, output 7441
- runs: 1
- milestone: M3
- result: NS_RobotDebris lifetime 4.9 s, collision gate and sink threshold 4.5 s (both expressions read back), compiles clean, saved; GDD enemy-death updated; not run in PIE; commit 7b752a0e

## Relations
- [[task-182-debris-pool-80-test]] — parent global task.
