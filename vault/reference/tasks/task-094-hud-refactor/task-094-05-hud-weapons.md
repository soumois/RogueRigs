---
id: task-094-05-hud-weapons
description: "Weapon slots, ammo, reload, turret charges and locked slots move into their own widget, centred as a row at the screen bottom."
parent: task-094-hud-refactor
kind: task
status: done
---
# WBP_Weapons

- goal: Weapon slots, ammo, reload, turret charges and locked slots move into their own widget, centred as a row at the screen bottom.
- agent: bp
- depends_on: task-094-04-hud-xp
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: create WBP_Weapons holding the four weapon slots (name, ammo bar, ammo blocks, reload bar, LOCKED) and the turret line, driven by OnAmmoChanged / OnWeaponSlotChanged and the reload and turret events; only the reload and recharge progress may tick, and only while one is running. Placement (owner wish): the whole row centred horizontally on the screen bottom (a HorizontalBox of four 160-wide columns anchored bottom centre, per the audit), above the XP bar and below nothing else; the subtitle widget (bottom centre, 900 x 80, bottom edge 50) must not overlap it: move WBP_Subtitles up so its bottom edge sits above the weapons row (about 140 from the bottom) and note it in wbp-subtitles. Remove the moved parts and the 185-call ammo update from the WBP_HUD tick. Owner rules (2026-10-11): WBP_HUD stays the single root that contains all sub-widgets and only places them; each sub-widget holds its own logic, binds to C++ events in Construct and unbinds in Destruct, no polling of unchanged values, a Tick only for a genuinely continuous value (stated in the audit); keep the current look and placement unless the audit says otherwise; no screenshot calls (nobody sees the screen: the owner judges); group the work, commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md and Tools/art/ are someone else's); quirks: real newline in pins, set_pin_value does not dirty the asset (touch a widget property and revert, then check git status shows the uasset changed), write_graph_dsl leaves stray nodes (read back and delete them); a new doc node for the widget in tech/blueprints linked from blueprints-index and wbp-hud updated (remove what moved). 40-turn limit: commit after each working group.
- done_when: compiled 0 errors, saved, row centred by values read back, subtitles moved clear of it, moved logic removed, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 60000
- model: sonnet
- tokens: input 106, cache write 136410, cache read 4830568, output 49813
- runs: 2
- milestone: M2
- result: WBP_Weapons (4 columns + turret line, bound to OnWeaponSlotChanged, 0.016 s timer only during reload/recharge) placed bottom centre (anchors 0.5/1, y -54); subtitles moved up (top -140); ammo update (185 calls) and turret/slot-name tick calls removed; no C++ lookup broken; not seen on screen. Commits c1521e9, e3c160b.

## Relations
- [[task-094-hud-refactor]] — parent global task.
