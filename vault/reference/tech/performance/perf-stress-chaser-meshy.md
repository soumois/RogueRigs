---
id: perf-stress-chaser-meshy
description: "Meshy chaser death-effects stress: S0, S2, S7, game 100-300 (task-181-01); debris pool 80 (182-03); GPU debris with walls, valid comparison (193-01)."
parent: performance-index
kind: doc
---
# Stress: Meshy chaser and death effects (2026-10-14, task-181-01)

Editor `-game` 1920 × 1080 through `Scripts/Run-StressTest.ps1` ([[sys-stress]]), HEAD c719274 (puff / debris counters, [[sys-stress-diagnostics]]). Owner everyday profile (Brave, Discord, Epic launcher, Zoom, NVIDIA Share, Parsec…), no game, no editor running. Not paced at 60 Hz (1800–3100 frames per 20–25 s). S0 / S2: 5 s settle, 20 s measure; S7: one launch per count, MinCells 8, 25 s; game: one launch per cap, settle 30, measure 60, `rr.Pacing.RunLookahead 0`. CSV `Saved/Stress/Stress_20261014_2214..2225*.csv`, logs `task181_*.log` (not committed). ms avg / p99 / max; draw, GPU avg / p99; M1 = [[perf-stress-final-m1]].

## S0 peace and S2 fire (chasers)
| N | S0 frame | S0 game | S0 draw | S0 GPU | M1 S0 frame | S2 frame | S2 game | S2 draw / GPU avg |
|---|---|---|---|---|---|---|---|---|
| 50 | 7.99 / 9.83 / 13.2 | 3.71 / 5.13 | 7.98 / 9.72 | 7.05 / 7.32 | 8.59 / 10.66 | 8.32 / 9.97 / 11.4 | 3.83 / 5.36 | 8.31 / 7.33 |
| 100 | 8.30 / 10.10 / 11.2 | 4.12 / 5.65 | 8.29 / 9.97 | 7.34 / 7.62 | 9.07 / 11.12 | 8.70 / 10.68 / 12.5 | 4.28 / 6.12 | 8.69 / 7.70 |
| 200 | 8.98 / 10.80 / 39.0 | 4.81 / 6.80 | 8.96 / 10.74 | 8.01 / 8.36 | 11.15 / 14.86 | 9.48 / 10.95 / 40.9 | 4.98 / 6.97 | 9.45 / 8.51 |
| 300 | 9.86 / 11.63 / 26.9 | 5.62 / 8.14 | 9.84 / 11.58 | 8.87 / 9.15 | 11.57 / 14.44 | 9.99 / 11.82 / 29.1 | 5.83 / 8.10 | 9.97 / 9.00 |
| 400 | 10.44 / 12.97 / 44.9 | 6.87 / 9.45 | 10.41 / 12.97 | 9.33 / 9.72 | 13.57 / 17.48 | 10.55 / 13.42 / 43.8 | 6.87 / 9.31 | 10.52 / 9.45 |
| 600 | 11.13 / 14.18 / 15.3 | 8.30 / 11.01 | 11.10 / 14.13 | 10.15 / 10.99 | 16.82 / 21.52 | — | — | — |

- Frame below M1 at every count (−0.6 to −5.7 ms), game thread 32–51 % lower (optimizations since M1). Now render-thread bound (draw ≈ frame): draw and GPU grow ≈ 5.6 µs per robot (+3.1 ms from 50 to 600), flat in M1 (draw 7.0, GPU 7.3 at 600). S2 adds ≤ 0.5 ms.
- Trace S0 600 (`-Trace cpu,frame,gpu,bookmark,region -statnamedevents`, one launch, frame 10.16 / 13.05): render thread visibility 2.1 ms, Lumen scene update 1.4 ms with ≈ 100 `MeshCardCapture` per frame (1.1 ms), primitive updates 0.86 ms per frame. Moving robot parts re-capturing Lumen cards is the likely per-robot cost; not proven (no A/B).

## S7 burst: N chasers killed in one frame + 16 barrels (M1: kamikazes)
| N | Expl. | Frame | Game | Draw p99 / GPU avg | Puff plays / skips | Debris plays / skips | M1 frame (game max) |
|---|---|---|---|---|---|---|---|
| 50 | 16 | 8.27 / 10.71 / 28.4 | 3.25 / 7.15 / 28.0 | 10.64 / 7.30 | 40 / 10 | 40 / 10 | 8.49 / 10.87 / 18.3 (15.9) |
| 100 | 16 | 8.07 / 11.23 / 27.1 | 3.31 / 7.55 / 16.0 | 11.08 / 7.12 | 40 / 60 | 40 / 60 | 8.73 / 14.16 / 29.5 (28.2) |
| 200 | 16 | 8.19 / 11.21 / 27.9 | 3.41 / 8.74 / 24.3 | 11.21 / 7.21 | 40 / 160 | 40 / 160 | 8.98 / 23.38 / 49.6 (47.5) |

- Chasers do not explode: 16 barrel explosions instead of 66–216, so the M1 chain p99 is gone. Death frame 27.0 (50) and 27.1 ms (100, game 16.0, draw 14.6); 200 deaths stay under 25 ms frame (game max 24.3). Barrel frames 25.5–28.4 ms at t 1.5–1.6 s. The cap holds: 40 puffs and 40 debris per frame at most, every other death of the frame skipped.

## Game mode (caps, one launch each)
| Cap | Alive avg (max) | Frame | Game | Draw / GPU avg | Spikes (gc / pause) | Puff plays / skips | Debris plays / skips |
|---|---|---|---|---|---|---|---|
| 100 | 73.8 (100) | 9.73 / 12.33 / 43.7 | 6.47 / 10.23 / 43.8 | 9.70 / 8.37 | 4 (1 / 2) | 277 / 0 | 278 / 2 |
| 200 | 165.6 (200) | 10.75 / 15.43 / 44.3 | 9.51 / 15.26 / 45.2 | 10.24 / 8.74 | 4 (1 / 2) | 633 / 36 | 465 / 204 |
| 300 | 232.3 (300) | 12.28 / 18.18 / 46.6 | 11.96 / 18.05 / 47.3 | 9.79 / 8.75 | 1 (1 / 0) | 975 / 11 | 531 / 441 |

- [[perf-stress-grid-cells]] game mode: 250 cap (alive 193–205) game 12.35–13.94 / 20.25–20.74 (task-105-01), 150 cap 10.4–10.8 / 19.4–20.1 (task-067-01). Now 300 cap (alive 232) 11.96 / 18.05: no regression within the ≈ 2 ms PC spread.
- Debris skipped for 1 %, 30 %, 45 % of deaths at 100 / 200 / 300: 40 components of 3.4 s serve ≈ 12 deaths per second; deaths come in bursts. Puff (0.3 s) skips only on mass kills.
- GC frames 39–47 ms (S0 / S2 at 200 and 400, game at t ≈ 31 s) as in M1. Untagged 27–29 ms frame at S0 / S2 300, t 16 s (game 20–22 ms), cause not traced.

## Verdict
The Meshy chaser and its death effects fit the budget: averages ≤ 12.3 ms and p99 ≤ 18.2 ms everywhere, burst frames ≤ 28.4 ms (M1: 49.6). The pool caps bound the death-effect cost; their price is visual (debris missing on 30–45 % of deaths above 200 alive). Lead: robot parts in the Lumen surface cache (render thread grows with the count).

## Debris pool 80 × 4.9 s against 40 × 3.4 s (2026-10-15, task-182-03)
Same launcher, map, resolution and options as above, HEAD 303fd1a (debris pool 80 on `BP_Chaser` and `BP_ShieldChaser`, puff pool 40, debris life 4.9 s = 4.5 s + 0.4 s sink, throw 325–600 / 350–550 uu/s, task-187 deposits). Owner profile: Discord, Epic launcher, Zoom, Parsec, NVIDIA container (Brave closed), no game, no editor. Not paced at 60 Hz (3039 frames in 25 s, 4630–5417 in 60 s). CSV `Saved/Stress/Stress_20261015_004941` (S7), `_005131` (game 200), `_005320` (game 300), `_005410` (memory run); logs `task182_*.log` (not committed).

| Step | Pool | Frame | Game | Draw avg / p99 | GPU avg / p99 | Alive avg | Puff plays / skips | Debris plays / skips |
|---|---|---|---|---|---|---|---|---|
| S7 200 | 40 × 3.4 | 8.19 / 11.21 / 27.9 | 3.41 / 8.74 / 24.3 | 8.17 / 11.21 | 7.21 / 7.84 | — | 40 / 160 | 40 / 160 |
| S7 200 | 80 × 4.9 | 8.23 / 11.56 / 25.9 | 3.67 / 9.00 / 23.2 | 8.21 / 11.43 | 7.12 / 8.26 | — | 40 / 160 | 80 / 120 |
| game 200 | 40 × 3.4 | 10.75 / 15.43 / 44.3 | 9.51 / 15.26 / 45.2 | 10.24 / 12.93 | 8.74 / 9.50 | 165.6 | 633 / 36 | 465 / 204 |
| game 200 | 80 × 4.9 | 11.08 / 15.85 / 46.7 | 9.88 / 15.77 / 47.6 | 10.59 / 13.34 | 8.46 / 9.54 | 158.1 | 640 / 49 | 587 / 73 |
| game 300 | 40 × 3.4 | 12.28 / 18.18 / 46.6 | 11.96 / 18.05 / 47.3 | 9.79 / 12.52 | 8.75 / 9.51 | 232.3 | 975 / 11 | 531 / 441 |
| game 300 | 80 × 4.9 | 12.96 / 19.36 / 51.5 | 12.52 / 19.34 / 51.4 | 10.63 / 13.44 | 8.61 / 9.52 | 245.6 | 933 / 130 | 760 / 300 |

- Every difference is inside the ≈ 2 ms PC spread: no Insights trace. Max frames are the GC frame (t 31 s, game mode) and the S7 death frame (25.9 ms, game 23.2, `xp drop 600`). Game 300 has 3 spikes (fill 1, gc 1, pause 1) against 1.
- Debris skips: game 200 30 % → 11 % of deaths, game 300 45 % → 28 %, S7 200 80 % → 60 % (80 per frame at most). Puff skips 36 → 49 and 11 → 130 (puff cap 40 unchanged; at 300 more deaths came in bursts).
- Task-187 (deposits break whole under explosions): S7 200 destroyed 1787 blocks against 1762 (+25), 21 event frames against 19, event max 21.5 ms (27.9 before). The burst comparison therefore includes slightly more block destructions; game-mode blocks depend on where the bot drives (12 / 48 here, 73 / 101 in task-181).

### Cost per extra active debris system
- Average active debris systems in game mode = plays × life / 60 s: 200 cap 26.4 → 47.9 (+21.5), 300 cap 30.1 → 62.1 (+32.0).
- Draw (render thread): +0.35 ms at 200, +0.84 ms at 300 (−0.07 ms for the 13 extra robots alive, 5.6 µs per robot from S0) → **16–24 µs per active system**; S7 draw p99 +0.22 ms for +40 systems → 6 µs. Upper bound 25 µs: all inside the spread.
- GPU: avg −0.28 / −0.14 ms, p99 +0.04 / +0.01 ms in game mode, S7 p99 +0.42 ms → **not measurable (≤ 10 µs per system)**.
- Game thread: +0.37 / +0.56 ms in game mode, mixed with alive count, explosions (41 / 80 against 36 / 73) and run variation; S7 avg +0.26 ms → **≤ 15 µs per system, not separable**.
- Memory (memory run: same S7 launch with `obj list class=NiagaraComponent` at start, 3.1 s after the spawn and 1.5 s after the burst, plus `fx.Niagara.DumpComponents filter=NS_RobotDebris`; frame numbers of that launch not used, the dumps cost up to 54 ms): pool components on the world settings 1 → 121 after the prewarm (1 damage numbers, 40 puff, 80 debris). An idle debris component is **3.58 KB** (NumKB; puff 3.28 KB); an active debris system adds **5.52 KB** of CPU particle data (ResExcDedSys, 2–4 particles: 34 × 2, 29 × 3, 17 × 4), video memory 0. So 40 extra components cost 143 KB idle and 364 KB all active (≈ 9.1 KB per active system). Process memory (`mem`: 3533 MB before, 3771 MB after the spawn and prewarm) mixes 200 robot spawns and is not usable. Mesh-renderer GPU buffers are not counted by `obj list` (unverified, render-thread dynamic data).

### Rule of thumb for raising the cap
- Debris systems needed ≈ deaths per second × 4.9 s, ×1.5 for bursts. Deaths per second (plays + skips over 60 s): 11 at 200 alive (needs ≈ 54 on average, 80 leaves 11 % skips), 18 at 300 alive (needs ≈ 87, 80 leaves 28 %); ≈ 130 would cover 300 alive.
- Each +40 of cap costs at most ≈ 1 ms render thread when all are active (25 µs each, game mode gives 0.64–0.96 ms per 40), no measurable GPU or game thread, and ≈ 0.36 MB. Raising 80 → 120 or 130 stays within the ≈ 2 ms spread; check with game 300 (render thread already at 10.6 ms avg).

## GPU debris with wall collision: trace findings (2026-10-15, task-191-02)
Its frame comparison ran on a slowed PC (GPU in software thermal slowdown, 60 fps pacing, a no-debris control S0 200 at 16.67 ms frame and 14.61 ms GPU) and is superseded by the task-193-01 section below. CSV `Saved/Stress/Stress_20261015_102223` to `_102945`, logs `task191_*.log` (not committed). Kept findings:
- Trace (one launch, game 200, `-Trace cpu,frame,gpu,bookmark,region`, `Saved/Profiling/Saved/Traces/task191_game200.utrace`, region `Stress_200_Measure`), GPU ms per frame against the task-181 trace S0 600 (no debris active), both halved because Insights records each GPU scope on two timelines (GPU1, GPU2; check: task-181 Frame scope halved = 10.13 against stat GPU 10.15): `GlobalDistanceFieldUpdate` **0.85** (absent in task-181), `DistanceFields` 0.91 against 0.06, `FXSystemPreRender` 0.35 against 0.11, `PostRenderOpsFX` 0.13 against 0.004, `NiagaraGpuComputeDispatch` 0.07 (absent). Two thirds of the GPU price of the debris is the global distance field the DF collision turns on (Lumen here does not build it otherwise). Not an A/B in one session (S0 600 against game 200).
- First-use hitch (before task-194): every launch logged `LogNiagara: Compiling System NS_RobotDebris took 0.12–0.21 s` at the first debris; in S7 it landed on the burst frame with `Waited for PSO creation for 200 ms` (frame 395.6 ms). Editor `-game` only.
- Debris counters (before task-194): S7 showed 200 plays and 0 skips with a cap of 80, because components waiting for the system compile were reclaimed and counted again. Fixed by task-194 ([[sys-actor-pool]]).

## GPU debris with walls against task-182-03, valid PC state (2026-10-15, task-193-01)
Same launcher, map, resolution and command lines as task-182-03 (S7 `-Counts 200 -Settle 5 -Measure 25 -Mode burst -Class Chaser -PreExec "rr.Stress.MinCells 8"`; game `-Settle 30 -Measure 60 -Mode game -PreExec "rr.Pacing.RunLookahead 0"`, one launch each), HEAD 70b18ef4, binary built after task-194 (the pool keeps compiling components, plays count only active effects, editor prewarm compiles `NS_RobotDebris`). Debris changes since 182-03: task-190 spin damping, task-191-01 GPU compute sim with distance field collision on walls and floor. Owner profile: Brave, Discord, Steam, Epic launcher, Zoom, Loom, ShareX, WhatsApp, uTorrent, NVIDIA Share; no game, no editor; AC power, Windows plan Performance, Armoury Crate set to Performance by the owner. CSV `Saved/Stress/Stress_20261015_151352` (control), `_151526` (S7), `_151728` (game 200), `_151919` (game 300), `_152128` (trace launch); logs `task193_*.log`, GPU samples `task193_nvsmi_control*.csv` (not committed). GPU max: the tool records none; the max of the spike lines is given.

### PC state control
- Before the owner's change: two controls S0 peace 200 gave frame 16.67 / 17.38 (60 fps pacing) and GPU 13.80 ms. During the second control the GPU ran at P3 780 MHz of 1785 and 52 W of 130 W at 60 °C, with clock event `0x20` (SW Thermal Slowdown) active. The Armoury Crate registry value `ROG ATKStatus` `ThrottleModeOnAC` read 2 (Silent in the asus-wmi convention). It still read 2 after the owner switched to Performance, so it does not reflect the mode.
- After the owner's change: S0 200 frame **8.65 / 10.84 / 15.47** (2311 frames in 20 s, not paced), game 4.43 / 6.16, draw 8.64 / 10.77, **GPU 7.63 / 7.98** (task-181: 8.98 / 10.80, GPU 8.01). Under load the GPU ran at P0–P3, 1530–1620 MHz, 118–129 W, 70–82 °C, clock event `0x4` (SW power cap), no thermal slowdown. Control passed.

| Step | Build | Frame | Game | Draw avg / p99 | GPU avg / p99 (spike max) | Alive avg | Puff plays / skips | Debris plays / skips |
|---|---|---|---|---|---|---|---|---|
| S0 200 control | 181 | 8.98 / 10.80 / 39.0 | 4.81 / 6.80 | 8.96 / 10.74 | 8.01 / 8.36 | 200 | — | — |
| S0 200 control | 193 | 8.65 / 10.84 / 15.5 | 4.43 / 6.16 | 8.64 / 10.77 | 7.63 / 7.98 | 200 | — | — |
| S7 200 | 182-03 | 8.23 / 11.56 / 25.9 | 3.67 / 9.00 / 23.2 | 8.21 / 11.43 | 7.12 / 8.26 | — | 40 / 160 | 80 / 120 |
| S7 200 | 193 | 8.90 / 16.55 / 54.1 | 3.67 / 9.24 / 27.3 | 8.86 / 16.25 | 7.38 / 10.15 (12.6) | — | 40 / 160 | 80 / 120 |
| game 200 | 182-03 | 11.08 / 15.85 / 46.7 | 9.88 / 15.77 / 47.6 | 10.59 / 13.34 | 8.46 / 9.54 | 158.1 | 640 / 49 | 587 / 73 |
| game 200 | 193 | 14.15 / 18.34 / 53.2 | 10.73 / 16.45 / 53.9 | 14.04 / 17.97 | 9.72 / 11.07 (9.4) | 163.9 | 646 / 23 | 627 / 44 |
| game 200 (trace launch) | 193 | 14.24 / 18.90 / 49.1 | 10.84 / 16.61 / 49.4 | 14.15 / 18.54 | 9.75 / 11.02 | 166.5 | 659 / 32 | 615 / 65 |
| game 300 | 182-03 | 12.96 / 19.36 / 51.5 | 12.52 / 19.34 / 51.4 | 10.63 / 13.44 | 8.61 / 9.52 | 245.6 | 933 / 130 | 760 / 300 |
| game 300 | 193 | 15.74 / 21.41 / 51.2 | 13.53 / 20.95 / 51.7 | 15.11 / 19.21 | 10.10 / 11.34 (11.0) | 235.7 | 987 / 90 | 804 / 273 |

- Game mode is worse than 182-03 by more than the ≈ 2 ms spread, and the extra time is on the render thread: frame +3.1 ms at 200 and +2.8 ms at 300; draw +3.5 / +4.5; GPU +1.3 / +1.5; game thread +0.9 / +1.0. Game mode is now bound by the draw thread (draw ≈ frame), where 182-03 was bound by the game thread. Active debris systems (plays × 4.9 s / 60 s): 51 at 200, 66 at 300, so the draw thread pays ≈ **68 µs per active GPU debris system** (182-03 CPU sim: 16–24 µs). Max frames are the GC frame (t 31 s), as in 182-03.
- S7 200: average +0.7 ms, but p99 +5.0 ms and max 54.1 ms (25.9). The worst frames come right after the 200 deaths (t 0.24–0.34 s: draw 18–54 ms, GPU up to 12.6 ms), when 80 GPU systems start within a few frames. No compile and no PSO wait on the burst: the system compiles at prewarm (frame 2, 0.53 s, before the measure), so the 395.6 ms task-191-02 hitch is gone (editor `-game`; cooked build not checked).
- Counters (real since task-194): S7 80 plays / 120 skips, which matches the cap. Debris skips: game 200 7 % of deaths (182-03: 11 %), game 300 25 % (28 %).
- Insights trace (one launch, game 200, `Saved/Profiling/Saved/Profiling/Saved/Traces/task193_game200.utrace`, region `Stress_200_Measure`, timing events per thread), ms per frame, against the task-181 S0 600 trace (no death, no debris active):

| Scope | Thread | 193 | 181 |
|---|---|---|---|
| `FGPUScene_UploadDynamicPrimitiveShaderDataForView` (incl.; nested `GPU` 1.94) | RenderThread | **2.04** | 0.01 |
| `FinishDynamicMeshElements` | RenderThread | **1.04** | 0.03 |
| `NiagaraGpuComputeDispatch` / `PostRenderOpsFX` | RenderThread | 0.53 / 0.32 | — / 0.06 |
| `UpdateGlobalDistanceField` | GPU graphics | 0.89 | — |
| `FXSystemPreRender` (Niagara GPU sim) | GPU async compute | 0.81 | — |
| `GPUScene.UploadDynamicPrimitiveShaderDataForView` | GPU graphics | 0.71 | — |

- Reading of the trace: about 3.6 ms of render-thread time per frame goes to Niagara GPU mesh-renderer work that is absent without debris. The largest part, the dynamic primitive upload, is where the engine runs GPU-scene writer delegates. In the engine source, a GPU-sim Niagara mesh renderer registers one writer delegate per renderer and view (`NiagaraRendererMeshes.cpp` `SetupElementForGPUScene`, `DataWriterGPU` → `AddUpdateMeshParticleInstancesPass`), so each active system adds a render-graph pass. Two points were not checked: whether the nested `GPU` scope is those passes, and how much comes from damage numbers rather than debris. The 181 trace is a different scene without deaths, so this is not an A/B. GPU side: global distance field 0.89 ms, the sim on async compute, and the instance writes on graphics.

### Verdict
With the PC at its reference speed (control passed), the GPU debris with walls costs more than the 80 CPU debris of 182-03:
- Game mode: average frame +2.8 to +3.1 ms and p99 +2.0 to +2.5 ms. Game 300 reaches 15.7 / 21.4 ms, above the task-181 verdict (averages ≤ 12.3 ms, p99 ≤ 18.2 ms).
- S7 death burst: worst frame 54 ms (182-03: 25.9 ms).
- Where the time goes: render-thread work per active GPU mesh system (≈ 68 µs each, ≈ 3–4 ms at 50–66 active), plus 1.3–1.5 ms GPU, of which the global distance field takes 0.9 ms.
- Fixed since task-191-02: the first-use compile hitch is gone, and plays/skips are now real.

Options for the owner, not decided or tested here: fewer active GPU systems (lower cap or shorter life), one shared system for many deaths instead of one component per death, or a CPU sim with a cheaper wall test.

## Relations
- [[performance-index]] — parent index.
- [[perf-stress-final-m1]] — the M1 S0, S2, S7 tables compared here.
- [[perf-stress-grid-cells]] — the game-mode results compared here.
- [[sys-stress-diagnostics]] — the puff and debris counters of the step line.
- [[sys-actor-pool]] — the Niagara pool and caps measured.
- [[enemy-death]] — the death effects measured.
- [[task-181-01-perf-chaser-meshy-death]] — the task that produced this report.
- [[task-182-03-debris-pool-cost]] — the debris pool 80 × 4.9 s comparison.
- [[task-191-02-debris-gpu-cost]] — the GPU debris with walls trace findings (its comparison is superseded).
- [[task-193-01-debris-gpu-cost-remeasure]] — the valid GPU debris with walls comparison and the PC state control.
