---
id: task-467-04-xp-rail
description: "Bp: The XP bar becomes a thin straight rail along the bottom edge with a framed Lv plate and clearly visible ticks."
parent: task-467-bottom-hud-match-target
kind: task
status: done
---
# Bottom HUD reworked in a capture loop until it matches the target image: bp

- goal: The XP bar becomes a thin straight rail along the bottom edge with a framed Lv plate and clearly visible ticks.
- agent: bp
- depends_on: task-467-03-slot-cells
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C5, C6 and C7 of the global acceptance hold at 1920 x 1080; XP fill, ticks at 20/40/60/80 %, gain ghost and card-pick flight to the XP bar work as before; umg-layout rules followed; compiles, no runtime error; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-467
- usage: included in the executor run of task-467
- runs: 1
- milestone: M2
- started: 2026-11-06T10:16:07Z
- waiting_for: none
- result: XPFrame = T_HUD_XPRail Box (840 x 58 native, margins 0.031 / 0.31, tint white), anchors 0,1-1,1, offsets 19 / -62 / 19 / 58 (x 19..1901, y 1018..1076); LevelCap = T_HUD_LevelPlate Box (216 x 55, 0.065 / 0.22) at 12, -63, 150 x 58; LevelText centred at 87, -34; XPBarArea 162 / -44 / 45 / 22 (bar in the rail window, y 1036..1058); ticks 3 px wide, inset 3, cream 1/0.95/0.85 opaque (was white 0.75, 2 px). Capture t467_04 1920x1080: straight rail with square riveted ends, Lv 1 in a framed plate, four ticks clearly visible at 20/40/60/80 %, block frame resting on the rail; after GrantXP 2 the pale ghost and the cyan fill show inside the rail. C5, C6, C7 seen; no script error. Card flight to the XP bar not caught on a frame (see 03). Commit 75272a91.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
