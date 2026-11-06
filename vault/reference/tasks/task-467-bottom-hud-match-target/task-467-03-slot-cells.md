---
id: task-467-03-slot-cells
description: "Bp: The four weapon slots become flat framed cells as in the target, contents unchanged."
parent: task-467-bottom-hud-match-target
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-467
- usage: included in the executor run of task-467
- runs: 1
- milestone: M2
- started: 2026-11-06T10:14:08Z
- waiting_for: none
- result: Frame0..3 background = T_HUD_SlotCell Box (160 x 82 native, margins 0.075 / 0.15), Col0..3 162 x 96 (was 150 x 90, gap 12 kept, row 684), Border content centred (padding 0, was top 4/4); WBP_HUD Weapons at -76 (row y 908..1004 inside the block frame). Capture t467_03 1920x1080: four equal flat cells with thin rims and small corner rivets, dark insides, rifle icon with its cream bar in cell 1, centred teal padlocks in 2-4: C3 seen. Card pick (t467_03f: GrantXP 6, ChooseUpgrade 0): screen closed, no script error; the flight itself fell between frames (not seen). Frame0..3 names and variables unchanged. Commit 9924d41b.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
