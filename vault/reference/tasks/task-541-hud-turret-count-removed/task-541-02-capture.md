---
id: task-541-02-capture
description: "Capture: Before/after HUD capture with the turret launcher owned at 1920 x 1080"
parent: task-541-hud-turret-count-removed
kind: task
status: done
---
# HUD: remove the "Turrets: 2 / 2" text above the health bar: capture

- goal: Before/after HUD capture with the turret launcher owned at 1920 x 1080
- agent: capture
- depends_on: task-541-01-remove-text
- inputs: Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: see done_when
- done_when: C1 no "Turrets" text above the health bar; C2 no grey marker left under where it was (per Q4); C3 the launcher slot charge bars visible; C4 health bar and slots unchanged against the before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-541
- usage: included in the executor run of task-541
- runs: 1
- milestone: M2
- started: 2026-11-13T23:13:45Z
- waiting_for: none
- result: -game L_ArenaM1 1920x1080, turret launcher given by rr.Debug.AddWeapon, 3 launches (before, after, after with 1 turret placed). C1 seen: no Turrets text above the health bar (Saved/Captures/t541_after/t541a_00.png). C2 seen: no grey marker. C3 seen: launcher slot shows its 2 charge blocks, and the red recharge block after one turret (Saved/Captures/t541_recharge/t541r_00.png). C4 seen: health bar and slots at the same place as the before frame (Saved/Captures/t541_before/t541b_00.png; crop sheet Saved/Captures/t541_after/slot_crops.png). ALERT at the previous closing: 45 turns over the 40 of light, cause: graph surgery through a hand-written JSON-RPC client (MCP tools not loaded) plus a third launch to check the recharge path I edited; 0.46 M units, under 4x.

## Relations
- [[task-541-hud-turret-count-removed]] — parent global task.
