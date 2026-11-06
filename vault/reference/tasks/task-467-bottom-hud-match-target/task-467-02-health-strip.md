---
id: task-467-02-health-strip
description: "Bp: the health bar becomes a thin framed strip on the slot row, its value text inside the green bar."
parent: task-467-bottom-hud-match-target
kind: task
status: todo
---
# Bottom HUD reworked in a capture loop until it matches the target image: bp

- goal: The health bar becomes a thin framed strip on top of the slot row with "100 / 100" inside the green bar, in the slim block frame.
- agent: bp
- depends_on: task-467-01-art
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1, C2 and C4 of the global acceptance hold in a PIE or -game frame at 1920 x 1080; health value and damage chunk work as before; umg-layout rules followed; compiles, no runtime error; committed.
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
