---
id: task-461-03-xp-frame
description: "Bp: The XP bar sits in its riveted frame with Lv in a framed cap."
parent: task-461-bottom-hud-panel
kind: task
status: done
---
# Bottom HUD built as one riveted panel and a framed XP bar: bp

- goal: The XP bar sits in its riveted frame with Lv in a framed cap.
- agent: bp
- depends_on: task-461-02-health-slots-panel
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), task-455-xp-bar-ticks (vault/reference/tasks/task-455-xp-bar-ticks.md), task-456-xp-gain-ghost (vault/reference/tasks/task-456-xp-gain-ghost.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; frame and cap shown, ticks and ghost unchanged in PIE with no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-461
- usage: included in the executor run of task-461
- runs: 1
- milestone: M2
- started: 2026-11-06T04:36:01Z
- waiting_for: none
- result: WBP_XPStatus: new XPFrame (T_HUD_XPFrame box 384 x 37, 0.06/0.3, tint 0.8, anchors 0,1-1,1, offsets 103/-47/7/37) over XPBarArea (now 122/-36/26/16), new LevelCap (T_HUD_LevelCap box 126 x 44, 0.2/0.3, tint 0.8, 108 x 44 at 8,-50) with LevelText centred on it (62,-28, align 0.5). XPGhostBar empty track tinted dark #0B0C14 as in the mockup. Ticks, ghost fill, XPBar name and graphs unchanged. Compiled, saved; PIE GrantXP 3 and 40 and two card picks: no Accessed None nor script error. Look not seen (A6). Commit 81462e37.

## Relations
- [[task-461-bottom-hud-panel]] — parent global task.
