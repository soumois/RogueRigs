---
id: task-415-02-aim-laser-component
description: "Cpp: replace the chevron decal of the miner by the aim laser component (Muzzle origin, stop at rock, 1200 uu max)"
parent: task-415-aim-laser
kind: task
status: todo
---
# Aim laser replaces the white aim chevrons: cpp

- goal: Replace the chevron decal of the miner by the aim laser: the aim indicator becomes a component drawing /Engine/BasicShapes/Plane with M_AimLaser (both loaded by C++ default path, BP_Miner not opened), no collision, no shadow. Each tick at TG_PostPhysics (after the hand pose): start at the shown rifle Muzzle ([[sys-weapons]], task-412), direction ARRMiner::GetAimDirection kept horizontal, height = Muzzle height (parallel to the floor, never entering it), width 19 uu (3 uu core + 8 uu glow each side), length = min(1200, ARRArenaGrid::GetClearDistance(Muzzle, aim, 1200)) (1200 without a grid), passes over robots. Hidden while the miner is dead (HandleDeath) or without material, shown whenever alive, mouse and stick alike. Push Color, EmissiveGain, Opacity, CoreFrac, GlowOpacity, TipFadeFrac from EditDefaultsOnly properties with the M_AimLaser defaults; MaxLength 1200 and Width 19 as EditDefaultsOnly. Remove the chevron decal code and every reference to M_AimChevrons; keep a BlueprintPure accessor on ARRMiner. No new test; fix only tests the change breaks (none reference AimIndicator today). Return a change summary (classes, properties, commit), no doc node.
- agent: cpp
- depends_on: task-415-01-mat-aim-laser
- inputs: task-415-aim-laser (vault/reference/tasks/task-415-aim-laser.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md)
- expected_output: Source/RogueRigs changes (aim indicator component and ARRMiner), Development Editor build succeeding, one commit by path; change summary.
- done_when: Development Editor build succeeds; RogueRigs automation tests that passed before still pass; Source has no reference to M_AimChevrons or the chevron decal; committed by path as `task-415-02-aim-laser-component: <summary>`.
- editor_state: Unreal editor closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-415-aim-laser]] — parent global task.
