---
id: task-431-02-packaged-green-square-doc
description: "Doc: record the packaged-build cause and fix in the decal warm-up docs"
parent: task-431-barrel-green-square-packaged
kind: task
status: done
---
# Barrel explosion green square in the packaged build: doc

- goal: Record the packaged-build cause and fix of the green decal square in the docs, from the cpp change summary.
- agent: doc
- depends_on: task-431-01-packaged-green-square-fix
- inputs: sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Updated sys-ground-telegraph (decal warm-up section: packaged cause, fix, verification), sys-explosions if its warm-up call changed, build-toolchain only if a packaging config or cvar changed; one commit by path.
- done_when: The cause, the fix and the packaged verification (frames, command line) are in sys-ground-telegraph; the other nodes are updated only where the change touched them; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: opus
- tokens: included in the executor run of task-431
- usage: included in the executor run of task-431
- runs: 1
- milestone: M2
- started: 2026-11-05T15:13:33Z
- waiting_for: none
- result: sys-ground-telegraph holds cause, fix and packaged verification, its null-material gotcha corrected; sys-cheats lists rr.Debug.FuseBarrel; sys-explosions and build-toolchain unchanged (warm-up call and packaging untouched); lint clean.

## Relations
- [[task-431-barrel-green-square-packaged]] — parent global task.
