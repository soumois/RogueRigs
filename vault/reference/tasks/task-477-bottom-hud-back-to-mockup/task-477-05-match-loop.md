---
id: task-477-05-match-loop
description: "Bp: Loop: -game capture, zoomed side-by-side with the mockup, rework failing points; at most 6 rounds."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: bp

- goal: Loop: -game capture, zoomed side-by-side with the mockup, rework failing points; at most 6 rounds.
- agent: bp
- depends_on: task-477-04-xp-rail
- inputs: task-467-bottom-hud-match-target (vault/reference/tasks/task-467-bottom-hud-match-target.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: Every point C1..C8 passes, or 6 rounds are done, or the owner said "je valide"; each round written in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:54:22Z
- waiting_for: none
- result: 2 rounds of 6, -game 1920x1080 captures (damage 30 + GrantXP 3), zoomed side-by-side sheets with the mockup at the same scale. Round 1 (Saved/Captures/t477_05_r1 = state after subtasks 02-04): C1 pass (block x 592..1331, top 842, strip 43 tall, rail 62 tall: within 1 px of the mockup), C2 C3 pass (pieces are the mockup's own), C4 pass, C6 C7 C8 pass; C5 failed on size: padlocks about 20 % smaller than the mockup's; also the empty XP track near-black navy where the mockup is dark grey. Change: LockImage0..3 63 x 63 (was 50), XPGhostBar background tint 0.019 grey (was #0B0C14), commit 80e03a8b. Round 2 (Saved/Captures/t477_05_r2): every point C1..C8 passes; no script error. Last sheets: Saved/Captures/t477_05_r2/sheet-block.png, sheet-bottom.png. Loop stopped (all pass).

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
