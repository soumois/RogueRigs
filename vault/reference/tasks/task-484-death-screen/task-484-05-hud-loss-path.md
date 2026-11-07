---
id: task-484-05-hud-loss-path
description: "Bp: HUD side of a loss: HUD hidden, old GAME OVER text and RUN LOST / Play again panel no longer shown; run-won and level-success end screens unchanged."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: bp

- goal: HUD side of a loss: HUD hidden, old GAME OVER text and RUN LOST / Play again panel no longer shown; run-won and level-success end screens unchanged.
- agent: bp
- depends_on: task-484-04-death-widget
- inputs: wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md)
- expected_output: see done_when
- done_when: In PIE a death and a missed shuttle show only the new screen (no GAME OVER text, no RUN LOST panel, HUD hidden); a level extraction and the run-won screen still show their panel and buttons; compiled with warnings as errors, saved, no script error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-484
- usage: included in the executor run of task-484
- runs: 1
- milestone: M2
- started: 2026-11-07T01:40:04Z
- waiting_for: none
- result: Commit 2b2bb7f4. WBP_AlertObjectives ShowRunEnd: Branch on bSuccess at the function entry, so only a success fills and shows EndScreen (run-won Main Menu and success panel unchanged); WBP_HealthStatus OnGameOver_Event_1 no longer shows GameOverText (its SetVisibility and getter removed, binding kept). HUD hidden on a loss by ARRPlayerController::HandleRunLost (03). Compiled with warnings as errors, saved. Checked in -game instead of PIE (no console command over MCP): death (Saved/Captures/rl04c_1080) and missed shuttle via rr.Run.Lost.Shuttle (Saved/Captures/rl05_shuttle) show only WBP_RunLost, no GAME OVER, no RUN LOST panel, HUD hidden, no script error. Success panel and button: RogueRigs.Integration.ConsoleExtractionWin passes (EndScreen visible, title, RestartButton click), with ShuttleTimeoutLoss, DeathRunLostScreen, ChasersKillMinerGameOver, Player.GameOver. Level extraction stats screen not touched. ALERT cause: executor run over the standard budget (MCP by hand, captures).

## Relations
- [[task-484-death-screen]] — parent global task.
