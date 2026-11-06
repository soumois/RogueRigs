---
id: task-467-02-health-strip
description: "Bp: the health bar becomes a thin framed strip on the slot row, its value text inside the green bar."
parent: task-467-bottom-hud-match-target
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-467
- usage: included in the executor run of task-467
- runs: 1
- milestone: M2
- started: 2026-11-06T10:04:13Z
- waiting_for: none
- result: HealthPanel (730 x 169, anchors 0.5,1, bottom -62) now holds BlockCanvas (CanvasPanel): HealthWell = T_HUD_HealthStrip frame (680 x 45 on top), HealthBarRoot inside its window (654 x 25), HealthText anchored on the bar centre, HealthPlate = T_HUD_BlockFrame (730 x 124) under it; Weapons row moved to -74 in WBP_HUD. Root cause of the floating text and bulky look found: Box borders draw at the texture's pixel size (ImageSize ignored), so the five sprites were re-cut at drawn size (470x45, 750x141, 160x82, 840x58, 216x55) and reimported. Capture t467_02d 1920x1080: strip framed, '100 / 100' and '60 / 100' inside the bar, damage chunk visible, block x 595..1325, top 849 (78.6 %), C1/C2/C4 seen; no script error. Commit 683d30e9.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
