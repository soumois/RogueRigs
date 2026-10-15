---
id: task-192-02-pickup-orientation-defaults
description: "BP: set the rest orientation mode on the XP nut, big XP chip and heal pickup Blueprints, docs updated."
parent: task-192-pickup-spawn-orientation-variants
kind: task
status: done
---
# Pickup orientation defaults on Blueprints

- goal: set the new rest orientation mode in the class defaults of BP_PickupXP (Full), BP_PickupXPBig (YawOnly: one interesting face, owner 2026-10-15) and BP_PickupHeal (Full); the values for the nut and heal pickups are provisional defaults the owner confirms at launch. BP_PickupOre (Full plus its 3 mesh variants) is set in task-189-05, once its meshes exist.
- agent: bp
- depends_on: task-192-01-pickup-spawn-orientation-variants
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md)
- expected_output: the three Blueprints compiled and saved by explicit path; bp-pickup-xp and bp-pickup-heal updated.
- done_when: Blueprints compile with warnings as errors; modes read back on each class default object; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: sonnet
- tokens: input 36, cache write 36428, cache read 432221, output 685
- runs: 1
- milestone: M1
- result: RestOrientation set and read back: BP_PickupXP Full, BP_PickupXPBig YawOnly, BP_PickupHeal Full, MeshVariants empty; compiled (no error returned, warning count not exposed) and saved; docs updated; not seen in PIE; commit 2deb17ef

## Relations
- [[task-192-pickup-spawn-orientation-variants]] — parent global task.
