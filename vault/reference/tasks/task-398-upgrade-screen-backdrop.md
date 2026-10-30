---
id: task-398-upgrade-screen-backdrop
description: "Global task: A full-screen black at 70 % opacity behind the Spare Parts shop screen (WBP_ShopScreen), as on the level-up cards; the finished level dimly visible."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Spare Parts screen: black backdrop at 70 %

- request: Owner, 2026-10-30, relayed by the main session (translated): during "the upgrade screen", put a black screen all behind it (first request; opacity superseded below). Q12 (which screen) was first answered "A : Choose an upgrade" (298a2a88); the owner then said "There was no black background for Choose an upgrade" and sent a screenshot of the SPARE PARTS screen (RIG SERVICE TERMINAL, WBP_ShopScreen, 5 cards, Reroll, Continue) with the arena fully visible and bright behind the panel, no dark layer at all: that is the screen he means (option B). Opacity, final owner decision 2026-10-30: "If the upgrade one is at 75%, you can put Spare Parts at 75% too", then, offered the choice, 70 %: the same value as the WBP_UpgradeChoice Backdrop (black alpha 0.7 in the vault). The vault said the WBP_ShopScreen Backdrop is black alpha 1, yet nothing dark shows (the owner hand-edited WBP_ShopScreen on 2026-10-29, d1c7766d): find why. The owner also sees no black backdrop on "Choose an upgrade" (WBP_UpgradeChoice) although the vault reads 0.7: check its real state and REPORT it to the main session without editing it, so the owner decides. WBP_UpgradeChoice and WBP_Intermission are not edited. One logical point, one global (this re-plan replaces the first task-398 nodes, not started).
- goal: Behind the whole Spare Parts shop screen (WBP_ShopScreen), a visible full-screen black layer at 70 % opacity over the finished level; nothing else of the screen changes; the real rendered state of the WBP_UpgradeChoice backdrop is reported, not changed.
- acceptance: bp: WBP_ShopScreen real state read and reported (Backdrop Border: visibility, brush colour and alpha, render opacity, anchors, z-order, what covers it) with the reason nothing dark showed; one full-screen black layer at alpha 0.70 behind every other child, reusing the existing Backdrop; cards, buttons, bindings, number sources and the task-396 labels unchanged; WBP_UpgradeChoice Backdrop state read and reported (rendered or not, and why), not edited; WBP_Intermission untouched; compiled with warnings as errors; saved; gdd run-shop and intermission say 70 % black over the level; docs updated; lint clean; committed by path; owner checks on screen himself; the final summary to the main session states the WBP_UpgradeChoice finding.
- gdd_check: Read 2026-10-30: [[run-shop]] and [[intermission]] (provisional: cards shop "on its own screen with an opaque black background", "the finished level is no longer visible"): conflict with the owner decision, changed 2026-10-30 to "black at 70 % over the finished level". [[xp-level-up]]: no backdrop value in the GDD (the line added by the first confirmation, option A, was removed). Tech: [[wbp-shop-screen]] (Backdrop Border black alpha 1, first child of Root, full screen; owner hand edit d1c7766d not documented), [[wbp-upgrade-choice]] (Backdrop Border black alpha 0.7 inside UpgradeScreen), [[wbp-intermission]] (0.85, not touched). Open values: none. Scope M2. Repo state 2026-10-30: task-397 (cpp, editor closed) runs first. Config/DefaultGame.ini and Config/DefaultEditor.ini hold uncommitted owner changes: never commit them (commit by path). No packaged build, no capture (owner).
- confirmed_by_owner: yes (2026-10-30, Spare Parts screen (screenshot), then 70 % chosen by the owner)
- milestone: M2
- next_step: task-398-01-shop-backdrop-70
- started: 2026-10-30T22:39:27Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner answers relayed by the main session).
- Q12 Quel écran : answered first A (Choose an upgrade), corrected by the owner with a screenshot to B, the Spare Parts screen (WBP_ShopScreen). Opacity: 70 %, the same as Choose an upgrade (owner choice).

## Subtasks
1. task-398-01-shop-backdrop-70 (vault/reference/tasks/task-398-upgrade-screen-backdrop/task-398-01-shop-backdrop-70.md) — bp: WBP_ShopScreen: find why no dark layer shows, black backdrop at 70 %; report the WBP_UpgradeChoice backdrop state without editing it
2. task-398-02-doc-shop-backdrop-70 (vault/reference/tasks/task-398-upgrade-screen-backdrop/task-398-02-doc-shop-backdrop-70.md) — doc: update wbp-shop-screen (70 % backdrop, cause) and note the WBP_UpgradeChoice finding in wbp-upgrade-choice

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-398-01-shop-backdrop-70]] — step 1, bp.
- [[task-398-02-doc-shop-backdrop-70]] — step 2, doc.
- [[wbp-shop-screen]] — the widget changed here.
- [[wbp-upgrade-choice]] — the level-up widget whose backdrop is only checked and reported.
- [[run-shop]] — the shop screen rule changed to 70 % black.
- [[task-397-shuttle-stays-gone]] — runs first; the takeoff the owner saw behind this screen.
