---
id: task-473-01-light-match-loop
description: "Bp: Loop: tune the miner light colour, intensity and radius, -game capture, side-by-side with image 2, rework until it matches or the Q2 cap."
parent: task-473-miner-light-calibration
kind: task
status: done
---
# Miner light calibrated on the owner in-game reference: bp

- goal: Loop: tune the miner light colour, intensity and radius, -game capture, side-by-side with image 2, rework until it matches or the Q2 cap.
- agent: bp
- depends_on: none
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: A2 to A5 and A7 pass on the last round capture, or the Q2 cap is reached with the remaining gaps listed; every round written in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-473
- usage: included in the executor run of task-473
- runs: 1
- milestone: M2
- started: 2026-11-06T18:22:25Z
- waiting_for: none
- result: Done at the Q2 cap (3 rounds), commit d2415904. FloorWallLight in BP_Miner set headless by Tools/unreal/miner_floor_light.py (location (0,0,0) untouched, A7; channel 1 only and castShadows true kept). Before: intensity 4, radius 800, exponent 4, colour 255/194/158 (pink pool, no orange centre, floor R p95 191 within 0.12h of the miner). Round 1: 8 / 500 / 4 / 255,170,60: orange centre seen; pool too small and steep; floor R p95 225 (burnt by the 205 rule); edge stays orange-red. Round 2: 6 / 700 / 3 / 255,160,90: pool size and falloff match image 2 (radial medians 0.20h 125 vs 128, 0.25h 101 vs 108); centre salmon-pink, not orange enough; R p95 215. Round 3 (kept): 6.5 / 700 / 3 / 255,175,60: warm orange centre (209,76,62 at 0.06h vs image 226,87,71), size and falloff kept, fades to red-magenta; A3 pass, A2 pass with gap (the outer ring is redder and darker than the pink-violet of image 2: the floor and ambient there are unchanged by Q1), A5 pass in frame (robots keep their colour), A4 open gap: floor R channel p95 217 / max 228 near the miner, above the about 205 rule, while image 2 itself reaches 252 R p95 (luminance about 103 vs 115 on image 2, so not brighter than the target). A8: same 58 startup error lines as before. Captures Saved/Captures/task473-before, -r1, -r2, -r3 with sheet_r1, sheet_r2, sheet_r3 (local, gitignored).

## Relations
- [[task-473-miner-light-calibration]] — parent global task.
