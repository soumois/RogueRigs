---
id: task-474-02-capture
description: "Capture: Check the smaller miner in game."
parent: task-474-miner-scale-down
kind: task
status: done
---
# Miner 10 % smaller: capture

- goal: Check the smaller miner in game.
- agent: capture
- depends_on: task-474-01-miner-scale
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 the miner is visibly smaller than on the look before, about 90 %; C2 rifle in hand, no floating or clipping part; C3 aim laser and muzzle flash start at the rifle muzzle; C4 numbers above the head not inside the mesh; C5 drone and camera unchanged; sheet before / after sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-474
- usage: included in the executor run of task-474
- runs: 1
- milestone: M2
- started: 2026-11-06T17:15:11Z
- waiting_for: none
- result: 2 launches (before at scale 1, after at 0.9, same commands: invulnerable, aim yaw 0, 2+2 chasers, DamageMiner 10, game view with HUD, close view pitch -20 arm 450, side view pitch -5 arm 350 yaw 90). C1 seen: miner clearly smaller against the same chasers, about 90 % (Saved/Captures/MinerScale474Sheets/side_before_after.png, side_crop.png). C2 seen: rifle held by both hands, no floating or clipping part (side_crop.png). C3 laser seen starting at the rifle front (side_before_after.png after); muzzle flash ambiguous: the bursts visible are robot hits, no flash caught at the muzzle in either run. C4 ambiguous: no number above the miner head in the frames (DamageMiner shows none; robot damage numbers fine); number heights are capsule based in C++, unchanged. C5 seen: game-view framing identical, drone same size (game_before_after.png with the owner reference). Sheets to send to the owner: Saved/Captures/MinerScale474Sheets/.

## Relations
- [[task-474-miner-scale-down]] — parent global task.
