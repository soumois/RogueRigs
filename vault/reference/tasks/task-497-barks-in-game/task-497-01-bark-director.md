---
id: task-497-01-bark-director
description: "Cpp: Barks shown on the key events through the subtitle subsystem, with global cooldown, alert priority and no repeat."
parent: task-497-barks-in-game
kind: task
status: in-progress
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
- waiting_for: none
- result: open

## Relations
- [[task-497-barks-in-game]] — parent global task.
