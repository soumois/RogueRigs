---
id: task-527-audio-zone-objective-complete
description: "Global task: One distance-faded sound when a zone objective (bonus chest, console hack) is validated."
parent: tasks-done-index
kind: task
status: done
---
# Son à la validation d'un coffre ou d'une console

- request: Owner 2026-11-13, play-test batch in PIE (12 points, family gros morceau split one point per global), with two screenshots (image 1 gameplay: HUD, alert bar C / B / ! / S, a chest light glow with no chest; image 2 Options, Sound tab, the "Sound" tab button drawn smaller than the other tabs). This point, verbatim: « Il faut un son pour lorsqu'on ouvre un coffre ou pirate une console ( tout ce qui demande de rester dans zone et qu'on la valide ), rajoute le principe de distance. ( même si la différence ne sera pas grande, cela crée de la profondeur ) ». Owner answer 2026-11-13 (relayed by the main session, verbatim): "tout par défaut" = the plan and the defaults of Q12, Q13, Q14 accepted.
- goal: Validating a zone objective plays one sound with distance (Q12 to Q14), through the distance sound base class of task-522 (runs after task-522-01).
- acceptance: A1 sound file chosen by the owner (Q14), kept and imported; A2 in a -game run with Verbose LogRRAudio, chest hack complete and console hack complete each log one play with its distance factor per Q13; A3 nothing plays while hacking; A4 build green, existing audio tests pass; sys-audio documents the event.
- gdd_check: Read audio-direction, sys-audio, objective-console, objective-bonus-chest. Two readings: a loop during the hack or a one-shot at validation; "qu'on la valide" read as the validation moment (Q12). Values open: Q13. Source: Q14. Edit: audio-direction. Depends on the base class of task-522.
- verification_level: standard
- family: comportement
- confirmed_by_owner: yes (2026-11-13, "tout par défaut")
- milestone: M2
- next_step: none
- started: 2026-11-13T16:15:32Z
- waiting_for: none
- executor_tokens: input 108, cache write 151032, cache read 3842383, output 24899
- executor_usage: runs 1, turns 52, context start 16k peak 120k, resumes 1, 100 min, units 0.70 M, USD 2.02
- planning_tokens: input 6, cache write 18666, cache read 174110, output 3853
- planning_usage: a163c449 "Plan owner feedback batch" 1/9 (tasks 520, 521, 522, 523, 524, 525, 526, 527, 528), units 0.06 M, USD 0.21
- result: Done. A1 owner ElevenLabs file (variant 2, most likely prompt P1) kept and imported as SW_ObjectiveComplete (placeholder). A2 -game: console hack complete 1 play at 400 uu fade 0.77, chest hack complete 1 play at 350 uu fade 0.83. A3 no play during either hack. A4 build green, RogueRigs.Console 6/6 (no Audio test family); sys-audio documents the event. ALERT at 01: resumed once (owner wait for the sound file, by design).

## Questions
Decided 2026-11-13 (owner "tout par défaut"): every default below is accepted; values provisional, owner 2026-11-13.
- Q12 Un son unique (pas de boucle pendant le piratage) au moment où la zone est validée : coffre ouvert (fin du piratage) et console piratée ; le même son pour les deux pour l'instant. Default: oui.
- Q13 Distance depuis le centre du coffre ou de la console : plein volume jusqu'à 200 uu, baisse linéaire jusqu'au silence à 1075 uu (comme la mort d'un robot). Default: oui.
- Q14 Le son : ElevenLabs, 3 prompts anglais écrits par l'exécuteur, tu choisis à l'oreille ; placeholder. Default: oui.

## Subtasks
1. task-527-01-sound-prompts (vault/reference/tasks/task-527-audio-zone-objective-complete/task-527-01-sound-prompts.md) — text: 3 English ElevenLabs prompts for the zone objective complete sound (chest opened, console hacked), owner generates and picks by ear
2. task-527-02-sound-import (vault/reference/tasks/task-527-audio-zone-objective-complete/task-527-02-sound-import.md) — bp: objective complete sound kept in Art/Source/audio with a manifest entry (placeholder), imported
3. task-527-03-complete-event (vault/reference/tasks/task-527-audio-zone-objective-complete/task-527-03-complete-event.md) — cpp: one distance sound at chest and console validation per Q12, Q13
4. task-527-04-doc (vault/reference/tasks/task-527-audio-zone-objective-complete/task-527-04-doc.md) — doc: sys-audio: the objective complete event

## Relations
- [[tasks-done-index]] — parent index.
- [[task-527-01-sound-prompts]] — step 1, text.
- [[task-527-02-sound-import]] — step 2, bp.
- [[task-527-03-complete-event]] — step 3, cpp.
- [[task-527-04-doc]] — step 4, doc.
