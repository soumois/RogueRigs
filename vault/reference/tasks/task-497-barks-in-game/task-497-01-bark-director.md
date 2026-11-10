---
id: task-497-01-bark-director
description: "Cpp: Barks shown on the key events through the subtitle subsystem, with global cooldown, alert priority and no repeat."
parent: task-497-barks-in-game
kind: task
status: wait-answers
---
# Rook and Vesna barks shown in game: cpp

- goal: Barks shown on the key events through the subtitle subsystem, with global cooldown, alert priority and no repeat.
- agent: cpp
- depends_on: none
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md), dialogue-tone (vault/reference/gdd/art-audio/dialogue-tone.md), crew-characters (vault/reference/gdd/crew-characters.md), Content/RogueRigs/Data/Text/barks.json
- expected_output: see done_when
- done_when: Acceptance A1 to A5 hold in a -game -nullrhi log run reaching at least a horde, the console and the extraction; cooldown is a data value; build clean, existing tests that the change breaks updated.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-10T20:24:05Z
- waiting_for: Owner decision (plan conflict A1 vs A3/Q2): horde_incoming, console_dropped and shuttle_landed each raise their own Kessler-Voss alert line in the same frame (horde_incoming, console_dropped, shuttle_countdown), so with alert priority and no queue their bark is always dropped (seen in 3 -game logs, Saved/Logs/bark-run*.log). Options: (a) the bark of these 3 events shows right after their alert line ends (a one-event exception to no-queue, still under the cooldown); (b) keep the rule: only console_hacked and bonus_chest_opened ever show barks; (c) other. Code committed 8517e040 (works for console_hacked and bonus_chest_opened; cooldown, alert replace, no repeat, other triggers refused: test RogueRigs.Subtitles.Barks passes).
- result: open

## Relations
- [[task-497-barks-in-game]] — parent global task.
