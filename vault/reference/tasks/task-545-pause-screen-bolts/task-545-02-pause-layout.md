---
id: task-545-02-pause-layout
description: "Bp: Rebuild the pause frame grid with the bolt and bar pieces as shared pieces"
parent: task-545-pause-screen-bolts
kind: task
status: done
---
# Pause screen: grid and bolts on the frame bars as owner image 10: bp

- goal: Rebuild the pause frame grid with the bolt and bar pieces as shared pieces
- agent: bp
- depends_on: task-545-01-bolt-art
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/9.webp, Saved/Scratch/owner-ui-batch-2026-11-13/10.webp
- expected_output: see done_when
- done_when: A1, A2, A4, A5 and A6 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-545
- usage: included in the executor run of task-545
- runs: 1
- milestone: M2
- started: 2026-11-14T03:42:13Z
- waiting_for: none
- result: Commit 8daaaa31. Shared widgets /Game/RogueRigs/UI/Shared/WBP_UI_BoltedBarV and WBP_UI_BoltedBarH (Root canvas: Bar image Box 3-slice, ends 0.05 unstretched, 16 slate thick; BoltStart/BoltEnd T_UI_Bolt 34x34 at the two ends, centred). WBP_PauseMenu: MinerDividerV and WeaponDividerV are BoltedBarV (x 0.3966 / 0.6065, frame top+20 to frame bottom-25 slate, bolts centred on the frame bars); WeaponDividerH is a full-screen CanvasPanel holding WeaponDividerHLeft (V1 to V2) and WeaponDividerHRight (V2 to frame right side -30 slate) at y 0.490; graph SetVisibility targets reconnected (two VariableGet nodes) after the widget swap. TitleBarL/R: T_Shop_TitleBar as Box (margins L 0.14 R 0.18, image 276x38), offsets +-162, size 475x38. Seen at 1920x1080 with 3 weapons: Saved/Captures/t545_r1_1080/pm_00.png.

## Relations
- [[task-545-pause-screen-bolts]] — parent global task.
