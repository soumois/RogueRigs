---
id: task-094-06-hud-cards
description: "The level-up cards and the reroll move into their own widget bound to the upgrade offer events."
parent: task-094-hud-refactor
kind: task
status: done
---
# WBP_UpgradeChoice

- goal: The level-up cards and the reroll move into their own widget bound to the upgrade offer events.
- agent: bp
- depends_on: task-094-05-hud-weapons
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: create WBP_UpgradeChoice holding the cards (name, value, description, Rook flavor line from the text after the first newline of DescriptionText, weapon cards, reroll if any) and their logic, shown on the offer event and hidden on the choice; WBP_HUD places it at z 10 as today; keep the card behaviour identical (styles, weapon card collapse rules, flavor line). Remove the moved parts from WBP_HUD. If a child widget instance with its own logic cannot be called from the HUD graph (the old note said so), report precisely how you solved it. Owner rules (2026-10-11): WBP_HUD stays the single root that contains all sub-widgets and only places them; each sub-widget holds its own logic, binds to C++ events in Construct and unbinds in Destruct, no polling of unchanged values, a Tick only for a genuinely continuous value (stated in the audit); keep the current look and placement unless the audit says otherwise; no screenshot calls (nobody sees the screen: the owner judges); group the work, commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md and Tools/art/ are someone else's); quirks: real newline in pins, set_pin_value does not dirty the asset (touch a widget property and revert, then check git status shows the uasset changed), write_graph_dsl leaves stray nodes (read back and delete them); a new doc node for the widget in tech/blueprints linked from blueprints-index and wbp-hud updated (remove what moved). 40-turn limit: commit after each working group.
- done_when: compiled 0 errors, saved, WBP_HUD places the widget, moved logic removed, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 60000
- model: sonnet
- tokens: input 100, cache write 151524, cache read 5006488, output 45485
- runs: 2
- milestone: M2
- result: WBP_UpgradeChoice (cards, reroll, flavor line; bound to OnUpgradeOfferShown/Closed; no tick; clicks via custom events) placed by WBP_HUD at z 10; old card functions and handlers removed; not run or seen on screen. Breaks C++ test lookups by name (RRIntegrationTests, RRWeaponsIntegrationTests incl. Integration.WeaponCardEquipsShotgun): to fix in task-094-10. Commits c0f0700, 7bf1eaf.

## Relations
- [[task-094-hud-refactor]] — parent global task.
