---
id: task-455-01-ticks
description: "Bp: The XP bar shows the 5-part ticks."
parent: task-455-xp-bar-ticks
kind: task
status: done
---
# XP bar split in 5 by light ticks: bp

- goal: The XP bar shows the 5-part ticks.
- agent: bp
- depends_on: none
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; ticks at the 4 fractions; PIE shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-455
- usage: included in the executor run of task-455
- runs: 1
- milestone: M2
- started: 2026-11-06T00:57:36Z
- waiting_for: none
- result: Commit 5e101e15. WBP_XPStatus: XPBar wrapped in CanvasPanel XPBarArea (takes the old XPBar slot: anchors 0,1-1,1, offsets 90,-36,20,16); XPBar fills it (anchors 0,0-1,1, offsets 0); four Images XPTick20/40/60/80 after XPBar (drawn above the fill, a ghost layer can slot between), anchors x 0.2/0.4/0.6/0.8 y 0-1, offsets left -1 top 3 width 2 bottom 3, white A 0.75, HitTestInvisible. Compiled, saved; graphs untouched (XPBar name kept). PIE L_ArenaM1 with GrantXP 3 then 40 (level 1 to 3): no Accessed None nor script error. Look not seen on screen (Q26: no capture), owner checks in game.

## Relations
- [[task-455-xp-bar-ticks]] — parent global task.
