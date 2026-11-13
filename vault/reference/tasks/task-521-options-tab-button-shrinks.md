---
id: task-521-options-tab-button-shrinks
description: "Global task: Defect: the clicked or selected Options tab button is drawn smaller than the other tabs."
parent: tasks-done-index
kind: task
status: done
---
# Options : l'onglet cliqué devient plus petit

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « image 2 : Tu peux voir que le bouton sound est plus petit, j'ai le même cas avec n'importe quel bouton lorsqu'on clique dessus. ( par exemple : Si je clique sur graphics, je vais avoir le soucis avec ce bouton et sound redevient normal ) » Screenshot: Options, Sound tab selected (cream plate), its button smaller than Graphics and Gameplay. Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q2, Q3 accepted.
- goal: Every Options tab keeps the same size as the others whatever was clicked or selected; the plate hover (1.05) still plays on hover.
- acceptance: A1 after clicking each tab in turn, the three tabs have the same drawn size (selected or not), outside hover; A2 Reset to defaults and Back keep their size after a click; A3 hover growth and selected look otherwise unchanged; A4 capture per Q3 compared with image 2; doc node of the widget updated.
- gdd_check: Read options-screen, ui-button-hover. No conflict: defect, the rule "growth never resizes" stands. Scope reading asked (Q2: tabs, or buttons of other screens too). Capture resolutions asked (Q3). No code read; not diagnosed.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-13T13:26:17Z
- waiting_for: none
- executor_tokens: input 110, cache write 93894, cache read 3546088, output 22744
- executor_usage: runs 1, turns 55, context start 16k peak 107k, resumes 0, 8 min, units 0.59 M, USD 1.63
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: Done. sys-options updated (commit 23950212): data line gives ActiveTabTransparentBorder 26 px and the margin basis; new gotcha on the tab size fix (commit 3548c421, same size for every tab, UV crop of the active plate); relation to task-521. sys-button-hover unchanged: the tabs are not attached to the hover subsystem. Lint clean.

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q2 Portée : les trois onglets Sound, Graphics, Gameplay de l'écran Options ; Reset to defaults et Back sont vérifiés aussi. Si tu vois le même défaut sur des boutons d'autres écrans, dis lesquels. Default: onglets Options seulement, Reset et Back vérifiés.
- Q3 Capture de vérification aux quatre résolutions habituelles (umg-layout). Default: les quatre.

## Subtasks
1. task-521-01-tab-size-fix (vault/reference/tasks/task-521-options-tab-button-shrinks/task-521-01-tab-size-fix.md) — bp: tabs keep the same size when clicked or selected
2. task-521-02-capture (vault/reference/tasks/task-521-options-tab-button-shrinks/task-521-02-capture.md) — capture: Options tabs after clicks, per Q3
3. task-521-03-doc (vault/reference/tasks/task-521-options-tab-button-shrinks/task-521-03-doc.md) — doc: widget doc node: the tab size fix

## Relations
- [[tasks-done-index]] — parent index.
- [[task-521-01-tab-size-fix]] — step 1, bp.
- [[task-521-02-capture]] — step 2, capture.
- [[task-521-03-doc]] — step 3, doc.
- [[sys-options]] — the system doc that records the tab size fix.
