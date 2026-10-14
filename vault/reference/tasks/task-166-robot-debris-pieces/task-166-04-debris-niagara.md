---
id: task-166-04-debris-niagara
description: "Build NS_RobotDebris to the C++ contract and wire it with the 13 pieces on BP_Chaser and BP_ShieldChaser."
parent: task-166-robot-debris-pieces
kind: task
status: in-progress
---
# Robot debris: Niagara system and Blueprint wiring

- goal: Build NS_RobotDebris to the C++ contract and wire it with the 13 pieces on BP_Chaser and BP_ShieldChaser.
- agent: bp
- depends_on: task-166-02-debris-code, task-166-03-debris-meshes-import
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), naming (vault/reference/tech/conventions/naming.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: NS_RobotDebris in /Game/RogueRigs/FX/ (or the folder of the existing FX): one CPU emitter, burst spawn of User.PieceMeshIndex length particles, mesh renderer with the 13 pieces in the report order selected by a MeshIndex particle attribute read from the array, position = system location + User.PieceOffset[i], velocity along User.HitDirection plus upward and random spread, random spin, gravity, collision with an analytic floor plane at the system Z only (no scene query: no interaction with miner, robots or blocks), then shrink and sink, lifetime 1.2 s in all, system finishes on its own (no loop); BP_Chaser and BP_ShieldChaser: DebrisSystem = NS_RobotDebris and DebrisPieces filled from debris_pieces.json (13 entries); doc nodes bp-chaser, bp-shield-chaser and a new fx doc node for NS_RobotDebris under blueprints-fx-index
- done_when: The system compiles; its user parameters and renderer meshes read back; the defaults on both Blueprints read back (13 pieces, system set); a short PIE probe killing a chaser shows a debris component active and released after about 1.2 s (values read, no screenshots); saved; docs updated; committed by path; the editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
