---
id: task-496-02-request-timing
description: "Cpp: Level 1 layout requested when the main menu opens and used at New Run; next level requested at the extractor board contact instead of later."
parent: task-496-generator-in-runs
kind: task
status: done
---
# Layout generator wired into real runs: cpp

- goal: Level 1 layout requested when the main menu opens and used at New Run; next level requested at the extractor board contact instead of later.
- agent: cpp
- depends_on: task-496-01-generator-provider
- inputs: sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), main-menu (vault/reference/gdd/run/main-menu.md), extraction (vault/reference/gdd/run/extraction.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: Acceptance A2, A3 and A6 hold in a -game -nullrhi log run: time stamps show level 1 requested at menu open and ready before or at New Run with no long game-thread frame, level 2 requested at the board contact, stats screen path, Continue and the task-318 rebuild unchanged; Retry and Main Menu then New Run draw a new seed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-496
- usage: included in the executor run of task-496
- runs: 1
- milestone: M2
- started: 2026-11-10T20:05:01Z
- waiting_for: none
- result: Done (commit 48b00b69). ARRMainMenuGameMode::StartPlay calls URRRunSubsystem::PrepareNewRunLayout when the menu opens (kept when ResetRun already prepared a run: Main Menu path). URRRunTransitionSubsystem::StartNewRun waits in stage WaitingForLayout (loading screen up in the old world) for a pending level 1 layout, then travels; the map load never waits for the generator. URRRunSubsystem measures the game-thread frames while a layout is built (core ticker; log 'game thread meanwhile: N frame(s), longest X ms'; frames over 100 ms logged; the first engine frame after start is logged and not counted). Next level: no code change needed: AdvanceToNextLevel -> RequestLayout already runs in the board contact frame (ARRShuttle::TryBoard -> Finish -> EndRun(true) -> HandleLevelExtracted); the terrain swap stays at the cards screen (task-318 path unchanged). Checks (Saved/Logs/task496/): timing.log + timing2.log -game -nullrhi from the menu: level 1 requested at menu open (frame 0) and ready before New Run ('already prepared'); board contact by rr.Debug.Objective Shuttle 0: 'miner extracted' and 'layout requested for level 2' in the same frame (20.10.18:505 frame 504), stats screen, Stats.Next -> rebuild 371 ms at the cards screen, Continue fade path; Retry after death drew run seed 296822788 (was 805611496) and waited 95 ms behind the loading screen; Main Menu drew 7624989, menu kept it, New Run used it. menu1-8.log: longest game-thread frame during generation 1.0-2.5 ms; the only frame over 100 ms is frame 1 (engine start, 125-150 ms with 0.6 s or 4.4 s generations alike). Build clean. Tests Integration LevelUpChoiceAfterIntermission, DeathRunLostScreen, ShuttleTimeoutLoss, ConsoleExtractionWin: 4/4 pass. Cost not measurable (see 01).

## Relations
- [[task-496-generator-in-runs]] — parent global task.
