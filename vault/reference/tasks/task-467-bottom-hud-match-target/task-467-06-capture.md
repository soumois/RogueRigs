---
id: task-467-06-capture
description: "Capture: Final independent check of the bottom HUD against the target, at 1920 x 1080 only (Q4, owner 2026-11-06)."
parent: task-467-bottom-hud-match-target
kind: task
status: done
---
# Bottom HUD reworked in a capture loop until it matches the target image: capture

- goal: Final independent check of the bottom HUD against the target, at 1920 x 1080 only (Q4, owner 2026-11-06).
- agent: capture
- depends_on: task-467-05-match-loop
- inputs: tool-capture (vault/reference/tech/pipelines/tool-capture.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: At 1920 x 1080 only (Q4, owner), one verdict seen / not seen / ambiguous with frame path for each point C1 one bottom block (health strip + slot row) centred horizontally (centre at 50 % +-2 % of screen width), about 38 % of screen width (target 31 %..69 %), its top at about 78 % of screen height, its bottom resting on the XP bar, total height about 16 % of screen height (+-3 %); C2 the health bar is a green fill in a thin framed strip (about 4 % of screen height) sitting directly on top of the slot row with no gap, a little narrower than the slot row; "100 / 100" is drawn INSIDE the green bar, centred and legible; no text floats above the block; C3 four equal framed cells side by side, wider than tall (about 3:2), thin gunmetal frames with small corner rivets and dark insides, flat (no rounded bolted plates); cell 1 shows the rifle icon with its cream reload bar under it inside the cell, cells 2-4 a centred teal padlock; C4 the frame around strip and cells is a thin riveted gunmetal frame with small rivets: no large round bolt heads, no bulky outer plate as in Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp; C5 the XP bar is a thin straight rectangular riveted rail along the bottom edge from about 1 % to 99 % of screen width, about 5 % of screen height, no arrow-shaped ends; "Lv 1" legible in a framed plate at its left end; C6 four light ticks clearly visible on the XP track at 20/40/60/80 %; C7 the bottom block does not cover the XP bar and nothing of the bottom HUD leaves the screen; C8 after damage the health text and fill update inside the bar (damage chunk visible); after an XP gain the fill and ghost show inside the rail; C8 checked with a damage and an XP gain triggered by debug commands; side-by-side sheets target / result produced for the owner; ambiguous points listed for the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-467
- usage: included in the executor run of task-467
- runs: 1
- milestone: M2
- started: 2026-11-06T10:19:45Z
- waiting_for: none
- result: 1 launch at 1920x1080 only (editor closed), Saved/Captures/t467_06_final (f_00 clean at 6 s, f_01 1 s after rr.Debug.DamageMiner 30 and rr.Cheat.GrantXP 3), no script error. C1 seen (f_00: block centred, x 599..1321, top y 851 = 78.8 %, height 167 = 15.5 %, bottom on the rail); C2 seen (thin framed strip on the cells, no gap, narrower than the cell frame, '100 / 100' centred inside the green bar, nothing above); C3 seen (4 equal flat cells about 1.6:1, thin rims, small corner rivets, dark insides, rifle + cream bar in cell 1, teal padlocks centred in 2-4); C4 seen (thin riveted frame, no big bolts, no bulky plate); C5 seen (straight rail x 19..1901, 58 tall = 5.4 %, square riveted ends, 'Lv 1' in a framed plate); C6 seen (4 light ticks at 20/40/60/80 %); C7 seen (block above the rail, nothing off screen); C8 seen (f_01: '70 / 100' inside the bar with the cream damage chunk, cyan fill and pale ghost inside the rail). Ambiguous: none. Sheets for the owner: Saved/Captures/t467_06_final/sheet-bottom.png (target / before / result / after damage+XP), sheet-full.png.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
