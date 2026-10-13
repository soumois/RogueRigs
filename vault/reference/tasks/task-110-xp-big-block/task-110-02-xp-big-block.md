---
id: task-110-02-xp-big-block
description: "A pooled big XP pickup Blueprint exists (bigger, bright white with a cyan glow) and is set as the big block class."
parent: task-110-xp-big-block
kind: task
status: todo
---
# Big XP block Blueprint

- goal: A pooled big XP pickup Blueprint exists (bigger, bright white with a cyan glow) and is set as the big block class.
- agent: bp
- depends_on: task-110-01-xp-big-block
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: UPDATED by the result of task-110-01 (commits e659abd, efca69e; read sys-pickups): the big block class is a field `XPBigPickupClass` (EditDefaultsOnly TSubclassOf<ARRPickup>, category Pickup|XP, default null = feature off) on the cube class itself. Create BP_PickupXPBig as a SIBLING of BP_PickupXP, deriving from the same C++ parent (PickupType XP, same XP pickup class family: copy the BP_PickupXP setup, NOT a child of BP_PickupXP, so it does not inherit XPBigPickupClass), with root template scale 1.8 x the cube (the cube root mesh is 0.3: the big block 0.54; the pool restores the root template scale, task-095), a material instance with a bright white emissive base and the cyan glow kept (trail / sparkle Niagara as the cube has, scaled if a parameter allows), the same pickup behaviour; leave XPBigPickupClass EMPTY on BP_PickupXPBig; then set XPBigPickupClass = BP_PickupXPBig in the class defaults of BP_PickupXP (this turns the feature on; no ini change). Compile; save; update bp-pickup-xp (new sibling, the field) and color-code if a precise value is chosen; commit. Do not change gameplay values. After it is on, a heavy (12 cubes) drops 1 big block + 4 cubes: report that the headless tests RogueRigs.PickupPool and RogueRigs.Integration should be re-run by the Orchestrator. Rules for Blueprint work: editor OPEN, MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs, a few large write_graph_dsl batches, stray events deleted by connection count, set_properties values as a JSON string, bOverride_* flags true for SizeBox values, dirty-asset trick before saving (check git status shows the uasset changed), no screenshot calls (nobody sees the screen), commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ may belong to someone else), 40-turn limit, no python3 (use node).
- done_when: BP_PickupXPBig compiled 0 errors and saved; set as the big block class; docs updated; lint clean; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-110-xp-big-block]] — parent global task.
