---
id: task-467-05-match-loop
description: "Bp: Loop: -game capture, side-by-side with the target, rework the failing points, until all pass or the round cap of Q3."
parent: task-467-bottom-hud-match-target
kind: task
status: todo
---
# Bottom HUD reworked in a capture loop until it matches the target image: bp

- goal: Loop: -game capture, side-by-side with the target, rework the failing points, until all pass or the round cap of Q3.
- agent: bp
- depends_on: task-467-04-xp-rail
- inputs: tool-capture (vault/reference/tech/pipelines/tool-capture.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: Each round: one -game capture at 1920 x 1080 through Tools/capture/capture.js, a side-by-side sheet target / capture, a verdict per point C1 one bottom block (health strip + slot row) centred horizontally (centre at 50 % +-2 % of screen width), about 38 % of screen width (target 31 %..69 %), its top at about 78 % of screen height, its bottom resting on the XP bar, total height about 16 % of screen height (+-3 %); C2 the health bar is a green fill in a thin framed strip (about 4 % of screen height) sitting directly on top of the slot row with no gap, a little narrower than the slot row; "100 / 100" is drawn INSIDE the green bar, centred and legible; no text floats above the block; C3 four equal framed cells side by side, wider than tall (about 3:2), thin gunmetal frames with small corner rivets and dark insides, flat (no rounded bolted plates); cell 1 shows the rifle icon with its cream reload bar under it inside the cell, cells 2-4 a centred teal padlock; C4 the frame around strip and cells is a thin riveted gunmetal frame with small rivets: no large round bolt heads, no bulky outer plate as in Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp; C5 the XP bar is a thin straight rectangular riveted rail along the bottom edge from about 1 % to 99 % of screen width, about 5 % of screen height, no arrow-shaped ends; "Lv 1" legible in a framed plate at its left end; C6 four light ticks clearly visible on the XP track at 20/40/60/80 %; C7 the bottom block does not cover the XP bar and nothing of the bottom HUD leaves the screen; C8 after damage the health text and fill update inside the bar (damage chunk visible); after an XP gain the fill and ghost show inside the rail; failing points reworked in the widgets (and sprites if needed, same rules) before the next round. Stops when every point passes, at the round cap of Q3, or on the owner "je valide". The result lists every round (failed points, what changed) and the last sheet path; if the cap is reached with failures, the remaining gaps are listed for the owner and nothing more is run. Committed after each round that changes an asset.
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
