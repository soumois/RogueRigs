---
id: task-467-03-slot-cells
description: "Bp: The four weapon slots become flat framed cells as in the target, contents unchanged."
parent: task-467-bottom-hud-match-target
kind: task
status: in-progress
---
# Bottom HUD reworked in a capture loop until it matches the target image: bp

- goal: The four weapon slots become flat framed cells as in the target, contents unchanged.
- agent: bp
- depends_on: task-467-02-health-strip
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C3 of the global acceptance holds at 1920 x 1080; slot icons, reload bar, padlocks and the card-pick flight to a slot work as before; umg-layout rules followed; compiles, no runtime error; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T10:14:08Z
- waiting_for: none
- result: open

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
