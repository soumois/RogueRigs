---
id: task-220-03-bp-shield-chaser-wiring
description: "bp: BP_ShieldChaser shows the combined mesh, swaps to plain clips and hides the shield on OnShieldBroken; shell removed; doc node rewritten."
parent: task-220-shield-chaser-integration
kind: task
status: in-progress
---
# BP_ShieldChaser: shield wiring

- goal: BP_ShieldChaser uses SM_ShieldChaser_Meshy_Anim and the new instances; the break swaps materials.
- agent: agent-bp
- depends_on: task-220-02-shield-chaser-material, task-220-05-shield-pavise-pose (first reimport in place SM_ShieldChaser_Meshy_Anim and T_ShieldChaserAnim from the rebaked files, keeping the 220-02 import settings, and check channel 2 again; editor-closed tool in `vault/reference/tech/pipelines/tool-unreal-reimport.md` if MCP cannot reimport)
- inputs: `vault/reference/tech/blueprints/enemies/bp-shield-chaser.md` (current state; the node is corrupted: a "- [[bp-robot-debris]]" line is repeated between every line, frontmatter included), `vault/reference/tech/blueprints/enemies/bp-chaser.md`, `vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md`, `vault/reference/agents/bp-mcp-lessons.md`, `vault/templates/blueprint.md`, the result of task-220-02.
- method: BodyMesh = SM_ShieldChaser_Meshy_Anim, slot 0 MI_ShieldChaser_Body, slot 1 MI_ShieldChaser_Shield, slot 2 MI_ShieldChaser_Emblem; no shadow as now; Custom Primitive Data unchanged (written by ARRChaser). Remove the ShieldShell component and its OnShieldBroken graph. New OnShieldBroken: set BodyMesh material 0 to MI_ShieldChaser_BodyPlain, materials 1 and 2 to MI_ShieldChaser_Hidden (pool reuse restores the captured materials, ARREnemy RestoreVisualParts). Do not touch BP_Chaser. Rewrite the doc node cleanly from the template (removes the corruption; keep bp-robot-debris once in Relations).
- done_when: compile and save without error; BodyMesh mesh and materials read back; a PIE spawn of one BP_ShieldChaser without runtime error if the tools allow it.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-220-shield-chaser-integration]] — parent global task.
