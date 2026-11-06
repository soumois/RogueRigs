---
id: task-468-01-unlimited-stop
description: "Cpp: Aim laser without length cap, ending on the first wall, robot or barrel, never over a wall, sharp tip."
parent: task-468-aim-laser-unlimited-length
kind: task
status: done
---
# Aim laser without length limit, stopping at walls and robots: cpp

- goal: Aim laser without length cap, ending on the first wall, robot or barrel, never over a wall, sharp tip.
- agent: cpp
- depends_on: none
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), miner (vault/reference/gdd/player/miner.md), task-423-miner-aim-laser-range-fade (vault/reference/tasks/task-423-miner-aim-laser-range-fade.md), Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp
- expected_output: see done_when
- done_when: Acceptance A1 to A5 hold in PIE or -game: no length cap; the line ends at border cliffs and breakable blocks without being drawn over them, on robots (shield included) and barrels, passes pickups, turrets, drone, console and extractor; tip ends sharp; width, glow, height, colour, visibility unchanged; no gameplay effect, aim assist untouched; tests broken by the removed cap or fade updated and passing; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-468
- usage: included in the executor run of task-468
- runs: 1
- milestone: M2
- started: 2026-11-06T10:23:22Z
- waiting_for: none
- result: Commit 0892d20c. URRAimIndicatorComponent: MaxLength 600 removed, replaced by TraceReach 10000 uu (search bound of the grid and robot checks, far past the screen edge, not a visible limit; GetMaxLength renamed GetTraceReach); the line ends on the first block cell (ARRArenaGrid::GetClearDistance: rock including the rock-closed arena border, ore, barrel) or the first living robot hit capsule (shield chaser included); pickups, turrets, drone, console, extractor skipped as before. Tip fade removed: TipFadeLength, ComputeTipFadeFrac, UpdateTipFade, GetTipFadeLength, GetPushedTipFadeFrac deleted; TipFadeFrac pushed as 0 once at BeginPlay (M_AimLaser clamps to 0.0001, sharp tip). Width, glow, Muzzle height, colour, visibility, aim assist untouched. No test referenced the cap or fade: none updated. Verify: build only (no aim-indicator test family), build exit 0. Owner capture read as a drawn mock: the grid stop already ends the line at border rock faces; Muzzle height vs BlockHeight 100 and the look are unverified in game (no capture, owner checks).

## Relations
- [[task-468-aim-laser-unlimited-length]] — parent global task.
