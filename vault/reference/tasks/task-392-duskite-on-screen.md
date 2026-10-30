---
id: task-392-duskite-on-screen
description: "Global task: Every player-facing "Ore" text becomes "Duskite": stats screen total label, run-won ore line, the extraction bark; code identifiers and vault ids unchanged."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# On-screen ore word: Duskite

- request: Owner, 2026-10-30, after testing the packaged build (Saved/Packaged/Windows/RogueRigs.exe), relayed verbatim by the main session (French), four points: "La lueur du coffre bonus reste entre les niveaux" / "Les particules de robot doivent aussi être retirées entre les niveaux (je pouvais voir des particules des robots tués au précédent niveau car j'ai passé rapidement à l'autre niveau)" / "Le dernier niveau doit ramener vers le main menu" / "Pour le dernier écran pour les ore sécurisés (ça n'est pas un autre mot que ore ?), il ne comptait pas ceux du level en cours." A screenshot of the arena was attached (aim chevrons visible, no issue there). Owner answer to his own question "ça n'est pas un autre mot que ore ?" (relayed 2026-10-30, translated): replace every on-screen occurrence of "Ore" by "Duskite" (stats screen "Total Ore:", end screens "Ore secured: N", HUD or shop texts, any other player-facing text, including Content/RogueRigs/Data/Text JSON and widget texts in the WBP); its own global task; find every player-facing occurrence first; do not touch code identifiers or vault ids. Duskite is a provisional name in the GDD ([[ore-blocks]]): the owner is told that a later rename means changing these texts again. Inventory (Planner, 2026-10-30, by reading the text files and the strings in the UI assets): screen_texts.json intermission_recap lines[0] "Total Ore:"; barks.json line 185 "Taxi is here. Tip is the ore."; WBP_AlertObjectives run-won line "Ore secured: " (built in ShowRunEnd); WBP_Intermission holds "Total Ore:" as a design-time text (the runtime text comes from screen_texts.json). Not player-facing (unchanged): tooltips or comments "Ore price of the card" (WBP_ShopCard), "ore balance" (WBP_ShopScreen), C++ UPROPERTY categories, log lines, test names. No C++ string shown to the player contains "Ore". The HUD ore counter and the shop balance show an icon and a number, no word. Exact wording asked in Q8. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q8 accepted.
- goal: No player-facing text says "Ore" any more: the stats screen reads "Total Duskite:", the run-won end screen "Duskite secured: N", the bark "Taxi is here. Tip is the Duskite."; nothing else changes.
- acceptance: text: screen_texts.json intermission_recap lines[0] "Total Duskite:" and barks.json line changed, validators pass; bp: WBP_AlertObjectives run-won ore label "Duskite secured: ", WBP_Intermission design-time label "Total Duskite:", each compiled with warnings as errors and saved; every other UI asset checked: no player-visible "Ore" left (list in the bp change summaries); no code identifier, asset name or vault id renamed; gdd intermission label updated (edits applied on confirmation; ore-blocks and glossary already state the rule); docs updated; lint clean; committed by path; owner checks on screen himself.
- gdd_check: Read 2026-10-30: [[ore-blocks]] (approved; "The ore is called Duskite (provisional name)"; only ore for now): no conflict; the on-screen rule written there and in [[gdd-glossary]] on 2026-10-30 after the owner answer; Duskite stays provisional, so a later rename repeats this task. [[intermission]] (provisional, label "Total Ore"): changed to "Total Duskite:" (Q8, edit applied on confirmation). [[dialogue-tone]]: the text agent applies the T rules to the changed bark. Tech read: [[sys-text-content]] (runtime loader of the text files), [[wbp-alert-objectives]], [[wbp-intermission]]. Open values: the exact wording (Q8). Scope M2. Order: after task-390, which changes WBP_AlertObjectives first (task-391 no longer touches that widget since its re-plan of 2026-10-30). No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-392-02-bark-duskite
- started: 2026-10-30T21:58:25Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q8 Les textes exacts : écran de stats « Total Duskite: » ; écran de fin « Duskite secured: N » ; réplique de la navette « Taxi is here. Tip is the Duskite. » (c'est un sous-titre à l'écran, donc changé aussi). Les infobulles d'éditeur, noms de variables et logs gardent « ore ». Default: ces trois textes.

## Subtasks
1. task-392-01-screen-texts-duskite (vault/reference/tasks/task-392-duskite-on-screen/task-392-01-screen-texts-duskite.md) — text: screen_texts.json: intermission_recap "Total Ore:" becomes "Total Duskite:"
2. task-392-02-bark-duskite (vault/reference/tasks/task-392-duskite-on-screen/task-392-02-bark-duskite.md) — text: barks.json: "Tip is the ore." becomes "Tip is the Duskite."
3. task-392-03-end-screen-duskite-label (vault/reference/tasks/task-392-duskite-on-screen/task-392-03-end-screen-duskite-label.md) — bp: WBP_AlertObjectives: run-won ore label "Duskite secured: "; check the other UI assets for a visible "Ore"
4. task-392-04-intermission-duskite-label (vault/reference/tasks/task-392-duskite-on-screen/task-392-04-intermission-duskite-label.md) — bp: WBP_Intermission: design-time "Total Ore:" text becomes "Total Duskite:"
5. task-392-05-doc-duskite-labels (vault/reference/tasks/task-392-duskite-on-screen/task-392-05-doc-duskite-labels.md) — doc: update the end screen and stats screen widget docs with the Duskite labels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-392-01-screen-texts-duskite]] — step 1, text.
- [[task-392-02-bark-duskite]] — step 2, text.
- [[task-392-03-end-screen-duskite-label]] — step 3, bp.
- [[task-392-04-intermission-duskite-label]] — step 4, bp.
- [[task-392-05-doc-duskite-labels]] — step 5, doc.
- [[ore-blocks]] — the Duskite name and the on-screen rule.
- [[gdd-glossary]] — the ore entry with the on-screen rule.
- [[intermission]] — the stats screen label changed here.
- [[task-391-final-screens-count-level-ore]] — previous task on the same end screen widget.
