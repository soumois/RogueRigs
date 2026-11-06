---
id: task-456-02-ghost-bar
description: "Bp: The XP widget gets the ghost bar behind the fill, in the retained look."
parent: task-456-xp-gain-ghost
kind: task
status: done
---
# XP gain ghost on the XP bar: bp

- goal: The XP widget gets the ghost bar behind the fill, in the retained look.
- agent: bp
- depends_on: task-456-01-ghost-logic
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; ghost driven by the C++; PIE XP gain and level-up show no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-456
- usage: included in the executor run of task-456
- runs: 1
- milestone: M2
- started: 2026-11-06T01:07:07Z
- waiting_for: none
- result: Done (commit 441acaae). WBP_XPStatus reparented to URRXPStatusWidget (set_parent); XPGhostBar ProgressBar added at index 0 of XPBarArea (behind XPBar, before the ticks; anchors 0,0-1,1, offsets 0, HitTestInvisible, fill linear 0.578/0.922/1 = #c8f6ff also set by C++, background = the default ProgressBar_Background white tint, i.e. the old XPBar empty look); XPBar background tint alpha 0 so the ghost shows where the fill is empty; ShowXP rewritten to set only LevelText (25 old nodes deleted; C++ owns both percents). Ticks and LevelText unchanged. Compiled true, saved, tree and graph read back. PIE L_ArenaM1: GrantXP 3 (lvl 1, 3/4) then 40 (lvl 3, 19/25), two card picks: no Accessed None, no script error. Not seen on screen (Q26: no capture).

## Relations
- [[task-456-xp-gain-ghost]] — parent global task.
