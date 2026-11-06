---
id: task-467-06-capture
description: "Capture: Final independent check of the bottom HUD against the target, at the resolutions of Q4."
parent: task-467-bottom-hud-match-target
kind: task
status: todo
---
# Bottom HUD reworked in a capture loop until it matches the target image: capture

- goal: Final independent check of the bottom HUD against the target, at the resolutions of Q4.
- agent: capture
- depends_on: task-467-05-match-loop
- inputs: tool-capture (vault/reference/tech/pipelines/tool-capture.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: Per Q4 resolutions, one verdict seen / not seen / ambiguous with frame path for each point C1 one bottom block (health strip + slot row) centred horizontally (centre at 50 % +-2 % of screen width), about 38 % of screen width (target 31 %..69 %), its top at about 78 % of screen height, its bottom resting on the XP bar, total height about 16 % of screen height (+-3 %); C2 the health bar is a green fill in a thin framed strip (about 4 % of screen height) sitting directly on top of the slot row with no gap, a little narrower than the slot row; "100 / 100" is drawn INSIDE the green bar, centred and legible; no text floats above the block; C3 four equal framed cells side by side, wider than tall (about 3:2), thin gunmetal frames with small corner rivets and dark insides, flat (no rounded bolted plates); cell 1 shows the rifle icon with its cream reload bar under it inside the cell, cells 2-4 a centred teal padlock; C4 the frame around strip and cells is a thin riveted gunmetal frame with small rivets: no large round bolt heads, no bulky outer plate as in Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp; C5 the XP bar is a thin straight rectangular riveted rail along the bottom edge from about 1 % to 99 % of screen width, about 5 % of screen height, no arrow-shaped ends; "Lv 1" legible in a framed plate at its left end; C6 four light ticks clearly visible on the XP track at 20/40/60/80 %; C7 the bottom block does not cover the XP bar and nothing of the bottom HUD leaves the screen; C8 after damage the health text and fill update inside the bar (damage chunk visible); after an XP gain the fill and ghost show inside the rail; C8 checked with a damage and an XP gain triggered by debug commands; side-by-side sheets target / result produced for the owner; ambiguous points listed for the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
