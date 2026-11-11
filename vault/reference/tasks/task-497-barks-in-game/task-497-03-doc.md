---
id: task-497-03-doc
description: "Doc: Document the bark flow: triggers, cooldown, alert priority."
parent: task-497-barks-in-game
kind: task
status: done
---
# Rook and Vesna barks shown in game: doc

- goal: Document the bark flow: triggers, cooldown, alert priority.
- agent: doc
- depends_on: task-497-02-capture
- inputs: sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), sys-subtitles (vault/reference/tech/systems/ui/sys-subtitles.md)
- expected_output: see done_when
- done_when: The nodes describe the triggers wired, the cooldown value and where it lives, and the alert priority; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-11T05:24:19Z
- waiting_for: none
- result: Commit 2e1e7755: sys-subtitles (barks bullet: triggers, BarkCooldownSeconds 30 and BarkTriggers / BarkTriggersAfterAlert as config of URRSubtitleSubsystem, C++ defaults; alert priority and replace; wait after alert; ShowBark callers; tests) and sys-text-content (PickBarkLine, bark display). Lint clean. Cost not recorded: agent-usage.js finds no transcript (known tooling issue).

## Relations
- [[task-497-barks-in-game]] — parent global task.
