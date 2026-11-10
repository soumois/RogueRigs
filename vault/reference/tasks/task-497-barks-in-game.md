---
id: task-497-barks-in-game
description: "Global task: Rare barks of Rook and Vesna (through the drone) shown as subtitles on key events, one line at a time, 30 s cooldown, alerts keep priority."
parent: tasks-to-do-index
kind: task
status: todo
---
# Rook and Vesna barks shown in game

- request: Context: read-only Code Architect scan of the GDD against the code (2026-11-10); the owner asked for questions on the top 4 gaps, answered them, then said: "lance les 4 tâches, je ne sais pas où sont génerés les json, il s'agit d'un script cpp qui s'en occupe". Point 2 (barks shown by the drone), owner decision: "Rare, une ligne à la fois": barks only on key events, global cooldown about 30 s, never during a Kessler-Voss alert (alerts keep priority), Vesna speaks only through the drone (L2). Owner answer 2026-11-10 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5 accepted.
- goal: The 34 existing lines of barks.json are shown in game, rarely: one line at a time on the key events, a global cooldown, never over a Kessler-Voss alert line, Vesna's lines as her voice through the drone.
- acceptance: A1 On each key event (horde_incoming, console_dropped, console_hacked, bonus_chest_opened, shuttle_landed) a bark of that trigger is shown in the existing subtitle widget with the speaker name, if the cooldown allows. A2 At most one bark on screen; after a bark no other for the cooldown (data value, 30 s provisional), measured in the log. A3 No bark while an alert line is shown; an alert arriving during a bark replaces it at once. A4 The line is drawn at random among the trigger's lines, never the same line twice in a row; text only, no sound. A5 No other trigger shows a bark; alert lines and their timing unchanged. A6 Owner-visible check: frames of a Rook bark, a Vesna bark and an alert alone.
- gdd_check: Read crew-characters, lore (L2), dialogue-tone (T1 to T9; barks M3). Targeted data read: Content/RogueRigs/Data/Text/barks.json, to list the 14 trigger ids for the key-event question (level_up, upgrade_offer, player_damaged, horde_incoming, console_dropped, console_hack_started, console_hacked, bonus_chest_dropped, bonus_chest_opened, extraction_ready, shuttle_landed, run_won, run_lost, game_over). Scan facts given by the main session: GetBark has no caller outside Text/ and tests; Show(SpeakerId) used only by ShowAlertLine; subtitle queue one line deep; no voice code on the drone. Conflict: dialogue-tone puts barks at M3; the owner pulls them in (asked, Q5). No open value. Lines unchanged, so no text or style-guide pass. Edit: dialogue-tone.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-10, "ok")
- milestone: M2
- next_step: task-497-01-bark-director
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-10 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-10.
- Q1 Événements clés : horde_incoming, console_dropped, console_hacked, bonus_chest_opened, shuttle_landed. Exclus : level_up, upgrade_offer, player_damaged (trop fréquents), console_hack_started, bonus_chest_dropped, extraction_ready (doublons d'un événement voisin), run_won, run_lost, game_over (écrans de fin). Défaut : oui.
- Q2 Collision avec une alerte Kessler-Voss : une bark qui tombe pendant une alerte est abandonnée (pas mise en attente) ; une alerte qui arrive pendant une bark la remplace tout de suite. Défaut : oui.
- Q3 Choix et affichage : une ligne au hasard parmi celles du déclencheur (Rook ou Vesna), jamais deux fois la même d'affilée ; texte seul, sans son ; dans le widget de sous-titres existant avec le nom du locuteur, comme les alertes ; rien de plus sur le drone. Défaut : oui.
- Q4 Cooldown : 30 s (valeur de données, provisoire), compté depuis la dernière bark affichée ; la première bark d'un niveau peut partir tout de suite. Défaut : oui.
- Q5 Les barks sont notées M3 dans le GDD ; on les fait maintenant. Défaut : oui.

## Subtasks
1. task-497-01-bark-director (vault/reference/tasks/task-497-barks-in-game/task-497-01-bark-director.md) — cpp: Barks shown on the key events through the subtitle subsystem, with global cooldown, alert priority and no repeat.
2. task-497-02-capture (vault/reference/tasks/task-497-barks-in-game/task-497-02-capture.md) — capture: One -game capture of barks and an alert in the subtitle widget.
3. task-497-03-doc (vault/reference/tasks/task-497-barks-in-game/task-497-03-doc.md) — doc: Document the bark flow: triggers, cooldown, alert priority.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-497-01-bark-director]] — step 1, cpp.
- [[task-497-02-capture]] — step 2, capture.
- [[task-497-03-doc]] — step 3, doc.
