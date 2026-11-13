---
id: task-524-03-doc
description: "Doc: wave director doc: first horde timing"
parent: task-524-first-horde-after-console
kind: task
status: done
---
# Première horde un peu après l'arrivée de la console: doc

- goal: wave director doc: first horde timing
- agent: doc
- depends_on: task-524-02-capture
- inputs: enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md)
- expected_output: see done_when
- done_when: A4 doc part holds; lint clean; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-524
- usage: included in the executor run of task-524
- runs: 1
- milestone: M2
- started: 2026-11-13T13:58:20Z
- waiting_for: none
- result: Done (0819ae59). Updated sys-spawner-director (console_timed_horde entry: ConsoleLandedDelaySeconds, SetConsoleExpected / NotifyConsoleLanded / GetHordeFrontTime, GetHordeConsoleDelay, landing and banner log lines, test DirectorConsoleTimedHorde, gotcha for test worlds), sys-spawner-director-defaults (first front 20 s after the console landing) and sys-hud-pressure (console-timed '!' fixed at 0.4 + 20/240). enemies-spawn-rules was already edited at confirmation. Lint clean (warnings only, none on these nodes). ALERT: executor run over the standard budget, cause in task-524-01.

## Relations
- [[task-524-first-horde-after-console]] — parent global task.
