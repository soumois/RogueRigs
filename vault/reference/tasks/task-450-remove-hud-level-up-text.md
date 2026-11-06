---
id: task-450-remove-hud-level-up-text
description: "Global task: The cyan LEVEL UP text of the HUD is removed; the gold text in the world stays the only one."
parent: tasks-done-index
kind: task
status: done
---
# Remove the cyan LEVEL UP text from the HUD

- request: Owner 2026-11-05 (verbatim): "Prépare les tasks pour faire que la particule de level up suive le joueur et retire le text Level up dans l'ui ( en bleu ). Prépare juste les tâches". This task is the point "retire le text Level up dans l'ui ( en bleu )". His screenshot: Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp. Owner answer 2026-11-05 (relayed by the main session, verbatim): "Ok, tout par défaut" = the plan and the defaults of Q22 accepted.
- goal: A level-up shows one LEVEL UP text only, the gold one in the world.
- acceptance: A1 in game, a level-up no longer shows the cyan "LEVEL UP" of Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp; A2 the gold text in the world, the XP bar and "Lv N" are unchanged; A3 the widget has no leftover timer or handler for it, and XP and level updates still work; compiles with no error and no runtime error on a level-up in PIE; A4 existing tests pass, only expectations this change breaks are updated; A5 No capture and no stress run in this task unless the owner asks: he validates the look in game himself; the final report lists what to look at and says plainly that the look is unverified until he has seen it. The report also gives the token cost of this task, per subtask and in total, from cost-log (units, turns, runs, resumes).
- gdd_check: Read xp-level-up (already gives the gold text only), wbp-xp-status: the cyan text is the LevelUpText TextBlock of WBP_XPStatus, Roboto Bold 56, cyan, shown 1 s on OnLevelUp, per its doc. No gdd edit. Does not touch the assets of tasks 446, 447, 448.
- confirmed_by_owner: yes (2026-11-05, "Ok, tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-05T22:51:16Z
- waiting_for: none
- executor_tokens: input 68, cache write 92986, cache read 2189787, output 20620
- executor_usage: runs 1, turns 34, context start 17k peak 105k, resumes 0, 5 min, units 0.44 M
- planning_tokens: input 7, cache write 35353, cache read 533294, output 7554
- planning_usage: aeb73265 "Plan FX list tasks" 1/16 (tasks 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450), units 0.14 M
- result: Done. A1: the cyan LevelUpText of WBP_XPStatus is removed (commit 00dd6187), so a level-up no longer shows it; look unverified until the owner sees it in game (A5, no capture). A2: XPBar, LevelText and the gold world text (NS_LevelUpText in BP_Miner, untouched) unchanged. A3: OnLevelUp binding, handler, HideLevelUp timer and function, Destruct timer clear and OnLevelUp unbind removed; compiled with no error; PIE level-ups (1 to 9 to 13) logged no runtime error. A4: asset-only change, no C++ or test touched, no test references LevelUpText; no test run. Docs updated (fd80fe7c). Cost: included in the executor run.

## Questions
Decided 2026-11-05 (owner "Ok, tout par défaut"): every default below is accepted; values provisional, owner 2026-11-05.
- Q22 Default: seul le texte cyan « LEVEL UP » de l'interface part (le widget WBP_XPStatus affiche ce texte) ; la barre d'XP, le « Lv N » et le texte doré restent.

## Subtasks
1. task-450-01-remove-text (vault/reference/tasks/task-450-remove-hud-level-up-text/task-450-01-remove-text.md) — bp: WBP_XPStatus no longer has the LEVEL UP text or its timer; the XP bar and level text stay.
2. task-450-02-doc (vault/reference/tasks/task-450-remove-hud-level-up-text/task-450-02-doc.md) — doc: Remove the cyan text from the docs that describe it as current.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-450-01-remove-text]] — step 1, bp.
- [[task-450-02-doc]] — step 2, doc.
