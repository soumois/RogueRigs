---
id: task-477-04-xp-rail
description: "Bp: The XP rail and Lv plate use the new pieces as on the mockup."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: bp

- goal: The XP rail and Lv plate use the new pieces as on the mockup.
- agent: bp
- depends_on: task-477-03-slot-cells
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: C6 holds on a -game frame at 1920 x 1080; XP fill, ticks and ghost unchanged; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:52:27Z
- waiting_for: none
- result: XPFrame = T_HUD_MockRail Draw As Image, anchors 0,1-1,1, offsets 140 / -68 / 15 / 62 (x 140..1905, y 1012..1074 at 1080p); LevelCap = T_HUD_MockLevelPlate Image, anchors 0,1, offsets 12 / -68, 156 x 63; LevelText centred at 90, -36.5, Roboto Bold 26 (was 22); XPBarArea in the rail window 166 / -47 / 58 / 25 (x 166..1862, y 1033..1058); ticks white (were cream), 4 wide, inset 2 (were 3 / 3), still at 20/40/60/80 %. Graphs unchanged. Capture t477_04 1920x1080: rail with riveted end caps along the bottom edge as the mockup, Lv 1 in its 8-rivet plate, four white ticks; after GrantXP 3 the pale ghost and cyan fill inside the rail (C6 seen); no script error. Commit 890572ff.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
