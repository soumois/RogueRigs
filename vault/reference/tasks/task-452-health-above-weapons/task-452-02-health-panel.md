---
id: task-452-02-health-panel
description: "Bp: The health panel takes the retained look, effects kept."
parent: task-452-health-above-weapons
kind: task
status: done
---
# Health bar just above the weapon slots: bp

- goal: The health panel takes the retained look, effects kept.
- agent: bp
- depends_on: task-452-01-art
- inputs: wbp-health-status (vault/reference/tech/blueprints/ui/wbp-health-status.md), sys-hud-health-bar (vault/reference/tech/systems/ui/sys-hud-health-bar.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved with no error; effects still driven; PIE hit shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-452
- usage: included in the executor run of task-452
- runs: 1
- milestone: M2
- started: 2026-11-05T23:45:51Z
- waiting_for: none
- result: Commit d6be3f29. WBP_HealthStatus: new HealthPanel (Overlay, variable) holds HealthPlate (Image, T_Shop_ButtonReroll Box brush, ImageSize 224 x 63, margins 0.15 / 0.28, slot Fill with padding -9 to swallow the sprite transparent rim), HealthBarRoot (moved in, Fill, padding 26 / 10, so bar about 600 x 24 inside the plate; shake and punch still on it) and HealthText (moved in, centred, Roboto Bold 16 white with 1 px black outline, was 24 below the bar). Panel 652 x 44 left at the old top-left spot until 03. End-screen graph now collapses HealthPanel instead of HealthBarRoot (plate would stay otherwise). Compiled with warnings as errors, saved. PIE in L_ArenaM1: chasers hit and killed the miner twice (hit, death, game over and restart ran), no LogScript error or Accessed None. Text kept white with outline, not the mockup black, so it stays readable over the dark empty part at low health. Look unverified on screen (Q26).

## Relations
- [[task-452-health-above-weapons]] — parent global task.
