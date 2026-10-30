---
id: task-399-total-duskite-number-overlap
description: "Global task: on the level-end stats screen the Total Duskite number is drawn over the end of its label; place it beside the label."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Stats screen: Total Duskite number overlaps its label

- request: Owner, 2026-10-30, relayed verbatim by the main session (French): "Bug visuel ( le nombre 14 est par dessus le "total duskit"". Screenshot attached (copied to `Saved/LevelEndScreen/task-399/owner-bug-before.webp`, not committed): the "LEVEL 1 CLEARED" stats screen (WBP_Intermission); in the top band of the frame, the purple ore icon, then the label "Total Duskite:" whose last letters ("ite") are covered by the number "14" (larger font). Everything else on the screen is fine. Planner cause (from [[wbp-intermission]] anchor_rule, not yet confirmed in the editor): OreLabel is anchored at x 0.43 with auto size and OreValue at a fixed x 0.525; task-392-04 lengthened the label from "Total Ore:" to "Total Duskite:" (Roboto Bold 32), so the label now runs past 0.525 and the number is drawn over it. No uncommitted owner edit of WBP_Intermission (git status 2026-10-30). Owner answer 2026-10-30 (relayed by the main session, verbatim): "Ok, par défaut" = the plan and the defaults of Q1, Q2 accepted.
- goal: On the stats screen the Total Duskite number sits right of its label "Total Duskite:" with a visible gap, never on top of it, whatever the label length; icon, label and number stay on one centre line in the top band of the frame; nothing else on the screen changes.
- acceptance: bp: WBP_Intermission OreIcon, OreLabel and OreValue laid out per Q1, fonts unchanged (label 32, number 48), texts and bindings unchanged, compiled with warnings as errors, saved, committed by path; capture (per Q2): at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 the number is fully right of the label with a gap, icon, label and number on one line inside the top band, the band does not overlap the horizontal divider, the rest of the screen as before; doc wbp-intermission anchor_rule updated; lint clean; committed by path.
- gdd_check: Read 2026-10-30: [[intermission]] (provisional): the top band holds the single Total Duskite line beside the ore icon, label and a larger number as the mockup (task-383 Q2, Q3): the request matches it, no conflict, no gdd edit. Rules versus rendering: layout fix only, no rule or text change. Tech read: [[wbp-intermission]] (OreIcon (0.40,0.31) 88 x 88, OreLabel (0.43,0.31) align (0,0.5) auto, OreValue (0.525,0.31) align (0,0.5) auto, task-383-02), [[umg-layout]] (rule: Canvas and anchors, no HorizontalBox to centre elements on a text line; the Q1 default is an exception the owner decides, the rule node is not edited). Open values: the fix method (Q1) and the capture (Q2). Scope M2. Repo state 2026-10-30: Config/DefaultGame.ini, Config/DefaultEditor.ini and vault/.obsidian/community-plugins.json have uncommitted owner changes: leave them untouched, commit by path.
- confirmed_by_owner: yes (2026-10-30, "Ok, par défaut")
- milestone: M2
- next_step: task-399-03-doc-total-duskite-row
- started: 2026-10-30T23:32:52Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "Ok, par défaut"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Comment corriger : (A) mettre l'icône, « Total Duskite: » et le nombre dans une seule rangée horizontale centrée dans la bande du haut, avec un petit écart (16) entre le texte et le nombre : le nombre suit toujours le texte, même si le nom Duskite change encore (exception à la règle umg-layout « pas de rangée », la règle elle-même n'est pas modifiée) ; ou (B) garder les ancres séparées et seulement décaler le nombre vers la droite (à refaire si le texte change). Tailles de police inchangées. Default: A.
- Q2 Vérification : une capture -game (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080) par l'agent capture, comparée à ta capture, ou tu vérifies toi-même en jeu (pas de sous-tâche capture). Default: capture par l'agent.

## Subtasks
1. task-399-01-total-duskite-row (vault/reference/tasks/task-399-total-duskite-number-overlap/task-399-01-total-duskite-row.md) — bp: WBP_Intermission: lay out OreIcon, OreLabel and OreValue so the number sits right of the label (Q1)
2. task-399-02-total-duskite-capture (vault/reference/tasks/task-399-total-duskite-number-overlap/task-399-02-total-duskite-capture.md) — capture: level 1 stats screen at four resolutions: number beside the label, nothing else moved
3. task-399-03-doc-total-duskite-row (vault/reference/tasks/task-399-total-duskite-number-overlap/task-399-03-doc-total-duskite-row.md) — doc: update the wbp-intermission doc: ore band layout after the fix

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-399-01-total-duskite-row]] — step 1, bp.
- [[task-399-02-total-duskite-capture]] — step 2, capture.
- [[task-399-03-doc-total-duskite-row]] — step 3, doc.
- [[wbp-intermission]] — the widget fixed here and its anchor_rule.
- [[intermission]] — the gdd look of the stats screen.
- [[umg-layout]] — the layout rule; Q1 default is an exception to it.
- [[task-392-duskite-on-screen]] — lengthened the label to "Total Duskite:", which caused the overlap.
- [[task-383-level-end-total-ore]] — set the current ore band anchors.
