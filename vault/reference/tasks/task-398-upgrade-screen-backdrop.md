---
id: task-398-upgrade-screen-backdrop
description: "Global task: A full-screen black at 25 % opacity behind the upgrade screen; which screen is asked (default: the level-up "Choose an upgrade" cards)."
parent: tasks-to-do-index
kind: task
status: todo
---
# Upgrade screen: black backdrop at 25 %

- request: Owner, 2026-10-30, relayed by the main session (translated): "During the upgrade screen, put a black screen all behind it with an opacity of 25%." Opacity 25 % decided. Which screen "the upgrade screen" is: asked in Q12. Vault state: WBP_UpgradeChoice (level-up "Choose an upgrade", during a level) already has a Backdrop Border black alpha 0.7; WBP_Intermission (stats screen) a Backdrop black alpha 0.85, the paused level showing through; WBP_ShopScreen (Spare Parts cards between levels) a Backdrop black alpha 1, and the GDD says that screen is opaque black ([[intermission]], [[run-shop]]), but the owner edited WBP_ShopScreen by hand on 2026-10-29 ("Send manual", d1c7766d), and in task-397 he saw the arena behind "the upgrade screen" after the end-of-level takeoff, which only a between-level screen shows. The bp agent reads the actual Backdrop opacity of the chosen widget before changing it. Reuse the existing Backdrop Border (no new widget). One logical point, one global. Owner answer 2026-10-30 (relayed by the main session, verbatim): "A : Choose an upgrade" = the plan and the defaults of Q12 accepted.
- goal: Behind the chosen upgrade screen (Q12), a full-screen black at 25 % opacity; nothing else of the screen changes.
- acceptance: bp: the chosen widget's Backdrop (full screen, black) read before and set to alpha 0.25 (or added if missing), read back; nothing else changed; compiled with warnings as errors; saved; gdd updated (edit applied on confirmation); docs updated; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[xp-level-up]] (no backdrop value in the GDD; the 0.7 lives only in [[wbp-upgrade-choice]]): default Q12 adds the 25 % rule there (edit). [[intermission]] and [[run-shop]] (provisional: cards shop "on its own screen with an opaque black background"): conflict if the owner means the shop (Q12 option B, then those nodes change). Stats screen backdrop 0.85 in [[wbp-intermission]] (not in the GDD). Fades of task-318 ([[sys-run-transition-fade]]) are full black transitions, not backdrops: not reused. Open values: which screen (Q12). Scope M2. bp subtask needs the editor open; runs after task-391-04 (cpp, editor closed). Repo state 2026-10-30: task-391-04 (cpp, editor closed) is running; this task runs after it. Config/DefaultGame.ini and Config/DefaultEditor.ini hold uncommitted owner changes: never commit them (commit by path). No packaged build (owner). No capture subtask (owner tests himself).
- confirmed_by_owner: yes (2026-10-30, "A : Choose an upgrade")
- milestone: M2
- next_step: task-398-01-backdrop-25
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "A : Choose an upgrade"): every default below is accepted; values provisional, owner 2026-10-30.
- Q12 Quel « écran d'amélioration » : A) l'écran « Choose an upgrade » des cartes de montée de niveau, pendant le niveau : il a déjà un fond noir à 70 %, qui passerait à 25 % ; B) l'écran des cartes Spare Parts entre deux niveaux : le GDD le dit sur fond noir opaque (100 %), vous l'avez retouché à la main le 29/10 ; il passerait à 25 % et le niveau terminé se verrait derrière (le GDD change) ; C) l'écran de stats après l'extraction (fond noir à 85 %). Vous avez vu la navette revenir « sur l'écran d'amélioration » après le décollage, donc plutôt B ou C. Default: A.

## Subtasks
1. task-398-01-backdrop-25 (vault/reference/tasks/task-398-upgrade-screen-backdrop/task-398-01-backdrop-25.md) — bp: chosen upgrade screen widget (default WBP_UpgradeChoice): Backdrop black at 25 % opacity
2. task-398-02-doc-backdrop-25 (vault/reference/tasks/task-398-upgrade-screen-backdrop/task-398-02-doc-backdrop-25.md) — doc: update the chosen widget doc with the 25 % backdrop

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-398-01-backdrop-25]] — step 1, bp.
- [[task-398-02-doc-backdrop-25]] — step 2, doc.
- [[wbp-upgrade-choice]] — the level-up cards widget of the default.
- [[wbp-shop-screen]] — the Spare Parts screen of option B.
- [[xp-level-up]] — the level-up rules that get the backdrop value.
- [[task-397-shuttle-stays-gone]] — the takeoff seen behind this screen.
