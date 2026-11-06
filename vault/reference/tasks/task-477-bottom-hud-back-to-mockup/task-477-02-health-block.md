---
id: task-477-02-health-block
description: "Bp: The health strip and the block frame use the new pieces, placed as on the mockup."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: bp

- goal: The health strip and the block frame use the new pieces, placed as on the mockup.
- agent: bp
- depends_on: task-477-01-art
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: C1, C4 hold on a -game frame at 1920 x 1080; health rules unchanged; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:46:12Z
- waiting_for: none
- result: HealthPanel 739 x 177 (anchors 0.5,1, align 0.5,1, offsets 1, -61): x 592..1331, y 842..1019 = mockup. BlockCanvas: HealthPlate = T_HUD_MockBlock Draw As Image (0,0-1,1, top 43); HealthWell = T_HUD_MockStrip Image (0,0-1,0, 16 / 0 / 18, h 43); HealthBarRoot in the strip window (55, 17, 59, h 24: 625 x 24 at 647,859); HealthText Roboto Bold 24 outline 2 at (-2, 29). Full-health stop of FillColorStops on the WBP defaults set to sRGB 38F435 (renders as the mockup green 30D22E through the bar shading; was 66D26F); the 0.5 and 0.2 stops and all health rules unchanged. Capture t477_02b 1920x1080: strip and block match the mockup position and look, '100 / 100' centred in bright green, after 30 damage '70 / 100' with the chunk inside the bar (C1 block part, C4 seen); no script error. Commit 707b6a53.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
