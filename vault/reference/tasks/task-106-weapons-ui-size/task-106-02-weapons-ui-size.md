---
id: task-106-02-weapons-ui-size
description: "The weapon slots show the placeholder weapon icons and the padlock instead of the weapon name and the LOCKED text."
parent: task-106-weapons-ui-size
kind: task
status: todo
---
# Icons in the weapon slots

- goal: The weapon slots show the placeholder weapon icons and the padlock instead of the weapon name and the LOCKED text.
- agent: bp
- depends_on: task-106-01-weapons-ui-size, task-108-02-weapon-icons-placeholder
- inputs: wbp-weapons (vault/reference/tech/blueprints/wbp-weapons.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md)
- expected_output: import the four PNGs of Art/UI/Icons as textures (Content/RogueRigs/UI/Icons/T_Icon_*, UI texture group, no mips, sRGB, through the editor MCP; if the MCP import tool cannot import external files, say so precisely and stop: do not work around it by hand) and use them in WBP_Weapons: in each slot cell the reserved 120 x 50 icon zone shows the weapon icon (burst rifle, shotgun, turret launcher chosen from the weapon of the slot: map by the weapon name/class that GetWeaponSlotHUD gives; unknown weapons fall back to the name text), the owned slot keeps a small name label under or over the icon only if the layout of task-106-01 leaves room (state what you did), and a LOCKED slot shows the padlock icon centred in the cell instead of the LOCKED text. The logic stays as it is (ShowSlotWidgets sets the image/visibility instead of the text); icons are placeholders: keep a single place (a map or a select) where a future final icon replaces them. Read every size back, compile 0 errors, save (dirty-asset trick, check git status shows the uassets changed), update wbp-weapons and wbp-hud notes; commit by name (new .uasset textures included); no screenshot calls. 40-turn limit.
- done_when: icons imported, shown in the slots by values read back, LOCKED slots show the padlock, compiled 0 errors, saved, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-106-weapons-ui-size]] — parent global task.
