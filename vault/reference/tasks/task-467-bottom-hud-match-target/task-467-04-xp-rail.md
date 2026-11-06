---
id: task-467-04-xp-rail
description: "Bp: The XP bar becomes a thin straight rail along the bottom edge with a framed Lv plate and clearly visible ticks."
parent: task-467-bottom-hud-match-target
kind: task
status: todo
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
