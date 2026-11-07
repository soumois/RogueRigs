---
id: task-484-03-death-flow
description: "Cpp: Death sequence, run-lost screen trigger for death and missed shuttle, recap values, Retry and Main Menu actions, screen texts."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: cpp

- goal: Death sequence, run-lost screen trigger for death and missed shuttle, recap values, Retry and Main Menu actions, screen texts.
- agent: cpp
- depends_on: task-484-02-elements
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), run-structure (vault/reference/gdd/run/run-structure.md), main-menu (vault/reference/gdd/run/main-menu.md), ore-economy (vault/reference/gdd/run/ore-economy.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-text-content (vault/reference/tech/systems/ui/sys-text-content.md), company-voice (vault/reference/gdd/company-voice.md)
- expected_output: see done_when
- done_when: Automated tests show: at 0 health controls are cut, time runs at 30 % for 1 s real time, then the screen event fires once with level reached, run robots destroyed and Duskite held, and gameplay pauses; a missed shuttle fires the same event with its own text id; Retry resets the run and starts level 1 through the loading screen; Main Menu resets the run and opens L_MainMenu; title and subtitle ids exist in the text file, checked by the style guide; tests broken by the change updated; build clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 16, cache write 19453, cache read 110612, output 5323
- usage: turns 7, context start 9k peak 23k, resumes 1 (message x1), 1 min, units 0.06 M
- runs: 1
- milestone: M2
- started: 2026-11-07T01:04:15Z
- waiting_for: none
- result: Commit 8af4aceb. URRRunLostSubsystem: death = level-up offer cancelled, time dilation 0.3 (URRRunSettings::DeathTimeDilation) for 1 s real time (DeathSequenceRealSeconds), then pause and OnRunLost(FRRRunLostRecap: level N of 4, run kills from the run log, Duskite held = carried + secured before the loss); missed shuttle = same event at once (run_lost_shuttle). Retry = URRRunTransitionSubsystem::StartNewRun (loading screen, level 1); Main Menu = URRRunFlowSubsystem::ReturnToMainMenu. URRRunLostWidget C++ base, ARRPlayerController RunLostWidgetClass (Z 45, HUD hidden), HUD end screen state moved to the run-lost moment, game mode 3 s restart timer removed, rr.Run.Lost.Die/Shuttle/Retry/MainMenu. Texts run_lost_death CONTRACT TERMINATED and run_lost_shuttle CLAIM SECURED (style guide: loop 0 fail L4 on CLAIM FORFEITED, corrected, loop 1 pass). Build clean; tests Player.GameOver, Integration.ChasersKillMinerGameOver, ShuttleTimeoutLoss, ConsoleExtractionWin, new DeathRunLostScreen, Text.* pass. Pause verified in the integration world; the loading screen path of Retry only in game (capture 06).

## Relations
- [[task-484-death-screen]] — parent global task.
