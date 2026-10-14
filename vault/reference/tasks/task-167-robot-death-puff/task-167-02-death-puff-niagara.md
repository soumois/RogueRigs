---
id: task-167-02-death-puff-niagara
description: "Build NS_RobotDeathPuff (about 12 sparks, 0.3 s, covering the robot) and set it as DeathPuffSystem on BP_Chaser and BP_ShieldChaser."
parent: task-167-robot-death-puff
kind: task
status: in-progress
---
# Robot death puff: Niagara system and wiring

- goal: Build NS_RobotDeathPuff (about 12 sparks, 0.3 s, covering the robot) and set it as DeathPuffSystem on BP_Chaser and BP_ShieldChaser.
- agent: bp
- depends_on: task-167-01-robot-death-puff
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), color-code (vault/reference/gdd/art-audio/color-code.md), naming (vault/reference/tech/conventions/naming.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: NS_RobotDeathPuff next to the other FX: one CPU sprite emitter, burst of about 12 sparks plus a short puff that covers a chaser (about 70 cm wide) during the 0.15 s shrink, lifetime 0.3 s, no loop, colours from color-code (sparks warm, no new colour meaning); DeathPuffSystem set on BP_Chaser and BP_ShieldChaser; a new fx doc node for it under blueprints-fx-index, bp-chaser and bp-shield-chaser updated
- done_when: The system compiles and finishes on its own (no loop); the defaults on both Blueprints read back; a short PIE probe killing a chaser shows a puff component active then released (values read, no screenshots); saved; docs updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-167-robot-death-puff]] — parent global task.
