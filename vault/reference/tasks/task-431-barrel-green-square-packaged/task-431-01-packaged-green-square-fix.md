---
id: task-431-01-packaged-green-square-fix
description: "Cpp: reproduce the green square in a packaged build from the second run of one process, find the cause, fix it, verify in packaged frames."
parent: task-431-barrel-green-square-packaged
kind: task
status: done
---
# Barrel explosion green square in the packaged build: cpp

- goal: Reproduce the green decal square in a packaged build from the second run of one process (owner: after a death and restart or a pause-menu exit and New Run it returns on every barrel explosion until the build is relaunched; a fresh launch is clean; seen in a non-Development package), name its cause with evidence, fix it, and show packaged frames without it in every run.
- agent: cpp
- depends_on: none
- inputs: task-201-01-decal-first-frame-green (vault/reference/tasks/task-201-decal-first-frame-green/task-201-01-decal-first-frame-green.md), task-200-01-decal-green-square-check (vault/reference/tasks/task-200-decal-green-square-check/task-200-01-decal-green-square-check.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Source/RogueRigs/Telegraph/RRDecalWarmUp.h`, task-431-barrel-green-square-packaged (vault/reference/tasks/task-431-barrel-green-square-packaged.md) `request` (owner answers)
- expected_output: Packaged-build frames and the log lines that show the cause (not committed, `Saved/Scratch/` or `Saved/Screenshots/`); C++ and, if needed, config fix; a change summary for the doc subtask (cause, fix, cvars or config changed, command lines used); one or two commits by path.
- done_when: (1) Before any change, reproduce in a staged package (BuildCookRun of build-toolchain, `Saved/StagedBuilds/Windows/RogueRigs.exe`): scenario in one process = run 1 with 2 barrel explosions, miner death, restart by the same path as the run-lost screen button (check that `rr.Run.Restart` is that path, else drive the button's function), run 2 with at least 3 barrel explosions, then pause-menu exit (`rr.Debug.PauseMenu exit`) and New Run (`rr.Menu.NewRun`), run 3 with 2 barrel explosions; frames at fuse start and explosion in every run. Owner: the square appears from run 2 on (death or pause-menu exit alike) and stays until the process is relaunched, so look for state kept across worlds (statics, game-instance or engine subsystems, the warm-up's per-world actor and material set, cached dynamic material instances, pooled decals or Niagara, PSO precache state). Try a Development package first; if clean, a Test package (Shipping-like, `rr.*` commands kept since they are compiled out only in Shipping); the green square is seen after the restart with its log lines, or, after 2 runs per configuration without it, the agent stops and reports (no blind fix). (2) The cause is named with evidence: why the task-201 warm-up (ARRDecalWarmUp, once per world, materials referenced for the world) does not cover the second and later runs of one process. (3) After the fix the same package configuration rebuilds with exit 0 and the same three-run scenario shows no green square at any fuse start or explosion. (4) A Shipping package (`-clientconfig=Shipping`) builds with exit 0 and is left staged for the owner's own check (Q6); its path and launch line are in the change summary. (5) The task-201 `-game` command still shows no green square. (6) Build exit 0; existing explosion, barrel and telegraph tests pass (only broken expectations updated, no new test). (7) Game rules unchanged; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: included in the executor run of task-431
- usage: included in the executor run of task-431
- runs: 1
- milestone: M2
- started: 2026-11-05T14:41:52Z
- waiting_for: none
- result: Reproduced in a staged Development package (twice): run 1 clean, runs 2 and 3 pale-green box of the decal over the whole fuse zone (1.5 s, 44 of 205 frames over 3 % pale green, up to 68 %), log 'decal material None'. Cause: URRGroundTelegraphComponent::ResolveDefaultMaterial cached the default material in a static weak pointer with bResolved true; a package collects it at the first map change (GIsEditor false, no keep flag, nothing holds it), the stale pointer gave null, SetTelegraphMaterial(nullptr) and the warm-up skipped, and FDeferredDecalProxy draws the engine default decal for a null material. Fix: resolve again when the weak pointer is stale (commit 1939e428; debug command rr.Debug.FuseBarrel 0dc9f49e). After the fix, same Development scenario: 194 frames in 3 runs, 0 with green, decal material MID_M_GroundTelegraph in each run, ring and burst present. Shipping package built exit 0, launch smoke 20 s OK, staged in Saved/StagedBuilds/Windows. -game check clean. 22/22 tests RogueRigs.GroundTelegraph, Heavy.GroundTelegraph, Barrel, BarrelFuse, Explosions pass. Game rules unchanged.

## Relations
- [[task-431-barrel-green-square-packaged]] — parent global task.
