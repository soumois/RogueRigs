---
id: task-497-01-bark-director
description: "Cpp: Barks shown on the key events through the subtitle subsystem, with global cooldown, alert priority and no repeat."
parent: task-497-barks-in-game
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-497
- usage: included in the executor run of task-497
- runs: 1
- milestone: M2
- started: 2026-11-11T05:11:41Z
- waiting_for: none
- result: Commits 8517e040 + 93f60be7. URRSubtitleSubsystem::ShowBark on horde_incoming, console_dropped, console_hacked, bonus_chest_opened, shuttle_landed (config BarkTriggers); cooldown BarkCooldownSeconds 30 (config, provisional); dropped while any line is shown; an alert replaces a bark at once; random line from both speakers, no immediate repeat (PickBarkLine); text only. Owner answer choix A: the 3 triggers with a same-frame alert (config BarkTriggersAfterAlert) wait for that alert line to end, then are tried under the cooldown. Build clean; RogueRigs.Subtitles + RogueRigs.Text pass (new RogueRigs.Subtitles.Barks). -game -nullrhi logs Saved/Logs/bark-run3.log, bark-run4.log: Vesna console_dropped bark after its alert, console_hacked dropped by cooldown, Rook shuttle_landed bark after its alert, horde alert replaces it, Rook bonus_chest_opened bark. Cost not recorded: agent-usage.js finds no transcript (known tooling issue).

## Relations
- [[task-497-barks-in-game]] — parent global task.
