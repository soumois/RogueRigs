---
id: task-106-01-weapons-ui-size
description: "Weapon slots resized like the DRGS reference at the bottom centre; column width bug fixed."
parent: task-106-weapons-ui-size
kind: task
status: done
---
# Weapons UI size

- goal: The weapon slots are bigger, like the DRGS reference (framed slots at the bottom centre), and the columns really have their set width.
- agent: bp
- depends_on: none
- inputs: wbp-weapons (vault/reference/tech/blueprints/wbp-weapons.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), wbp-subtitles (vault/reference/tech/blueprints/wbp-subtitles.md), hud-refactor-audit-core (vault/reference/tech/blueprints/hud-refactor-audit-core.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-weapons (vault/reference/tech/systems/sys-weapons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: owner capture 2026-10-12 (Reference/DRGS/ui-weapons-current-2026-10-12.webp, 1926 x 1032 window): the row is centred but far too small: "Burst rifleLOCKEDLOCKEDLOCKED" is one crammed line about 346 px wide with a tiny ammo bar under the first slot, although the asset says four 160-wide columns (about 640 px): so first find why the SizeBox width (WidthOverride 160 on Col0..3) is not honoured in the rendered widget (the text spans measured in the capture: Burst rifle about 99 px, each LOCKED about 81 px, i.e. the columns collapsed to their text; check WidthOverride vs MinDesiredWidth, the slot Size rule of the HorizontalBox children (Auto vs Fill), the parent SizeBox of the whole row, auto-size of WBP_Weapons in WBP_HUD, DPI scale) and fix the cause. Then resize to the owner reference (Reference/DRGS/ui-weapons-reference-2026-10-12.webp, DRGS: four framed slots side by side at the bottom centre, each about 110-130 px wide in a 1749 px frame, i.e. a pitch of about 140-145 px in our 1920 reference, the icon big, a coloured status bar right under each slot, the locked slots show a lock and their unlock level, the whole group centred under the health bar). Proposed values (owner to confirm, pending): slot cell 150 x 90 px, gap 12 px, row about 636 px wide; each slot a dark translucent frame (0/0/0/0.45) with a 2 px outline (white-gold 1/0.9/0.5 when owned, grey when LOCKED); the weapon name at the top of the cell in the icon position (Roboto Bold 22, centred, wrapped to 2 lines max) with a reserved 120 x 50 icon zone so that a weapon icon can replace the name later without moving anything (no icon asset exists yet); ammo bar 130 x 14 or the 8 ammo blocks 14 high under the name, reload bar the same place; LOCKED text 20 grey centred; keep the bottom edge 54 px above the screen bottom, centred; the turret line stays above the row (centred); move WBP_Subtitles up in WBP_HUD so its bottom edge is above the turret line and the row (about 190 px from the bottom) and note it in wbp-subtitles. Do NOT touch the health bar or the XP bar. Keep all the logic (ShowSlot, ShowSlotWidgets, ProgressStep, events) as it is: this is layout and look only; read every size back from the asset; compile 0 errors; save (dirty-asset trick, check git status shows the uasset changed); update wbp-weapons and wbp-hud notes (placement table) and wbp-subtitles; no screenshot calls (nobody sees the screen: the owner judges); MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs, stray events deleted by connection count, a few large batches; commit by name after the change (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ belong to someone else). 40-turn limit. Do not call python3 (a Windows Store alias hangs): use node.
- done_when: cause of the collapsed columns found and fixed; slot sizes read back from the asset match the values above (or the owner values); no overlap computed with subtitles, turret line and XP bar (1920 x 1080 reference); compiled 0 errors; saved; docs updated; lint clean; committed.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Cause: SizeBox bOverride_WidthOverride flags were false so the 160 px columns collapsed to their text; fixed (and BarBox); cells 150 x 90, gap 12, row 636 px, frames, IconZone 120 x 50, bars 130 x 14, subtitles top -190; read back, saved, not seen on screen. Commits 4251176, d4d5744.

## Relations
- [[task-106-weapons-ui-size]] — parent global task.
