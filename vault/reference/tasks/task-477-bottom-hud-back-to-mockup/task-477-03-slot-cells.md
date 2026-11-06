---
id: task-477-03-slot-cells
description: "Bp: The four weapon cells use the new cell piece as on the mockup, contents unchanged."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: bp

- goal: The four weapon cells use the new cell piece as on the mockup, contents unchanged.
- agent: bp
- depends_on: task-477-02-health-block
- inputs: wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: C5 holds on a -game frame at 1920 x 1080; slot rules and card flight unchanged; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:49:42Z
- waiting_for: none
- result: Frame0..3 background = T_HUD_MockCell Draw As Image (163 x 108); Col0..3 SizeBoxes 163 x 108 (were 162 x 96), gap 4 (was 12): row 664 wide; WBP_HUD Weapons top -72 (was -76): cells at x 628, 795, 962, 1129, y 900..1008 = mockup (628, 796, 963, 1130). Contents, names (Frame0..3 flight targets) and graphs unchanged. Found: the WBP_HUD HealthStatus instance stored the old FillColorStops as an override once WBP_HUD was resaved, so the bar went dull again (t477_03); set on the instance too. Capture t477_03b 1920x1080: four equal framed cells wider than tall, dark insides, rifle with cream reload bar in cell 1, teal padlocks centred in 2-4 (C5 seen); bright green kept; no script error. Card flight not caught on a frame. Commit 6bb459e9.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
