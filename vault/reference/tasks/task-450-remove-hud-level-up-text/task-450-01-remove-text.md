---
id: task-450-01-remove-text
description: "Bp: WBP_XPStatus no longer has the LEVEL UP text or its timer; the XP bar and level text stay."
parent: task-450-remove-hud-level-up-text
kind: task
status: done
---
# Remove the cyan LEVEL UP text from the HUD: bp

- goal: WBP_XPStatus no longer has the LEVEL UP text or its timer; the XP bar and level text stay.
- agent: bp
- depends_on: none
- inputs: wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp
- expected_output: see done_when
- done_when: Compiled and saved with no error; the text, its timer and its hide function are gone; a level-up in PIE logs no runtime error; XP and level still update.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-450
- usage: included in the executor run of task-450
- runs: 1
- milestone: M2
- started: 2026-11-05T22:51:16Z
- waiting_for: none
- result: Commit 00dd6187. WBP_XPStatus: LevelUpText widget removed; HideLevelUp function removed; OnLevelUp_Event_2 handler (SetVisibility + SetTimerbyFunctionName) and the AssignOnLevelUp bind removed (AssignOnXPChanged now chains to ShowXP); Destruct ClearTimerbyFunctionName removed; UnbindXP keeps only the OnXPChanged UnbindEvent; two empty stray OnLevelUp_Event custom events and two orphan Self nodes deleted. CompileWidgetBlueprint true, saved, graphs read back. PIE: two GrantXP 300 cheats took the miner to level 9 then 13 with two card picks; log has no Accessed None, LogScript or LogBlueprint error. HUD look unverified by a frame (A5: owner checks it).

## Relations
- [[task-450-remove-hud-level-up-text]] — parent global task.
