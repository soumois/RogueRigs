---
id: task-084-01-card-flavor-fix
description: "The upgrade card flavor line is visible in game: the newline delimiter bug in WBP_HUD GetCardFlavor is fixed."
parent: task-084-card-flavor-fix
kind: task
status: in-progress
---
# Card flavor line fix

- goal: The upgrade card flavor line is visible in game: the newline delimiter bug in WBP_HUD GetCardFlavor is fixed.
- agent: bp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/sys-level-up.md), sys-text-content (vault/reference/tech/systems/sys-text-content.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: owner playtest: no flavor line on any card. Diagnosed root cause (task-080-01 agent, from the saved asset): in WBP_HUD function GetCardFlavor the ParseIntoArray Delimiter pin holds the two characters backslash-n instead of a real newline (write_graph_dsl does not unescape \n), so the split returns one element and the flavor is empty. Fix: set the Delimiter to a real newline (set_pin_value with an actual newline, or split on the first newline another way), compile, save. Add one line to unreal-mcp-quirks: the DSL does not unescape \n in string literals. Update wbp-hud doc. No screenshot calls. Verify by reading the pin default back from the graph and the asset; the owner confirms on screen (GrantXP cheat then level-up cards).
- done_when: Delimiter pin reads back as one real newline character; compile 0 errors; asset saved; wbp-hud and unreal-mcp-quirks updated; lint clean; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-084-card-flavor-fix]] — parent global task.
