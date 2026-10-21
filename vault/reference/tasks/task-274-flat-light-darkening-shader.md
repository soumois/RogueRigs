---
id: task-274-flat-light-darkening-shader
description: "Global task: post-process mask becomes a pure darkener (gain 1 at the miner, no tint); flat light raised so the unmasked screen looks like the old centre."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Flat light and darkening-only shader

- request: "le shader doit juste être un obscurant. Si je retire le shader, je devrais avoir ce que j'ai au centre de l'écran (lorsqu'il est activé), mais sur tout l'écran. Je ne sais pas pourquoi tu y as rajouté un effet de teinte." and "On rajoutera une light par la suite sur le joueur qui ne touche que le sol et le mur, mais on va procéder par étape. On doit d'abord avoir un rendu correct de lumière sans cette light, seulement via la light générale et le shader." (owner, 2026-10-21, formalised by the main session)
- goal: M_PP_PlayerLightMask outputs SceneColor x lerp(FarBrightness, 1, mask), mask = 1 / (1 + (r / Radius)^Exponent), no tint and no gain above 1; the flat light is raised so that with Enable = 0 the whole screen looks like the masked centre; the far falloff matches the DRGS ring profile by measurement.
- acceptance: MPC_PlayerLight holds only Enable, FarBrightness, Radius, Exponent, PlayerPosition (removed CenterBrightness, WarmTint, FarTint, TintStrength documented with previous values); the C++ writer names no removed parameter; exposure and sun read back and documented before and after; -game frames A, B, D, E, F, H with Enable 1 and Enable 0: masked centre disc (r < 120 px) and the whole unmasked frame within about 10 percent in mean luminance and of the same hue; far floor about 9-12/255; centre not clipped above about 205/255; XP, crystals, robots, thrusters and HUD comparable with final_272_flat_*; side by sides Saved/LookTune/final_274_*.png; docs updated; lint clean; committed.
- gdd_check: read the task-272 nodes, mpp-player-light-mask, lights-dark-ambient, ppv-xp-glow (2026-10-21). No rule conflict: lighting values are provisional and owner-tuned; the earlier "warm pool" look is replaced by the owner's decision (no tint, colour from the flat light only). No player light in this task (the owner adds one later).
- design: exposure is the lever. Today the centre is scene x 30 x WarmTint (luminance factor about 0.83), about x25 = +4.7 EV, so exposure min = max 1.8 -> -2.9 EV100 gives the same centre with gain 1. Emissives were multiplied by the same mask, so the exposure path keeps them where they were (near: x25 then x1; far: x25 then x0.005, as the old x0.15); raising the sun instead would leave emissives about 25 times too dark against the scene. FarBrightness starts at 0.15 / 30 = 0.005, Radius 200, Exponent 3, then calibrated by measurement. Sun 4.0 kept (floor normal map relief).
- confirmed_by_owner: yes (2026-10-21, relayed by the main session with its defaults)
- milestone: M1
- next_step: task-274-07-far-floor-sweep
- result: Reopened 2026-10-21: task-273-04 measured the far floor (floor pixels only) at 1.8/255 against 9-12; round 10 had scored all pixels, biased by the rock tops; steps 07-08 added (capture round 3). Before reopening: Mask is a pure darkener (gain = lerp(FarBrightness, 1, mask), no tint); MPC_PlayerLight CenterBrightness 30, TintStrength 1, WarmTint (1, 0.9, 0.06), FarTint (1, 0.7, 0.55) removed; FarBrightness 0.15 -> 0.002, Radius 200 -> 260, Exponent 3 -> 4, Enable 1; exposure min = max 1.8 -> -3.4 EV100 (-2.9 then +0.5 from the round-10 sweep); sun 4.0 and sky 0.08 unchanged; C++ writer cleaned. No emissive material changed. Round 11 (Saved/LookTune/r11) at asset values (no override; Enable 0 frames via bDriveTunables in a second run). Scores A 0.277, B 0.710, D 0.224, E 0.230, F 0.414, H 0.550 (r9 A 0.223, D 0.123). A rings 167 140 99 54 26 16 11 14 15 15 10 7; D 166 150 103 70 36 17 11 14 12 12 8 7. Centre r<120 (on) vs whole Enable 0 frame, sRGB luma / linear: A -6.0/-13.8, B -2.7/-7.4, D -6.6/-15.6, E -5.0/-12.2, F -12.5/-25.9, H +6.1/+10.3 percent; hue 282-286 vs 283-284 deg. Centre disc on vs same disc off: -3 to -8 percent linear. p99 162-238 (rock tops, robots). XP cyan kept (top-20 luma 197 vs 204), far XP dim; crystals hue kept, luma 150 vs 168; robots white-cream instead of gold. Differs: rock tops near the miner much brighter than the darker task-273 floor (white in B, F, H); centre violet-magenta (sun colour) instead of DRGS warm; tail past ring 4 steeper than DRGS. Side by sides Saved/LookTune/final_274_{A_idle,B_wall,D_robots_xp,E_xp,F_deposits,H_dig}.png.

## Subtasks
1. task-274-01-mask-writer-cleanup — cpp, remove the tint and CenterBrightness tunables from the writer.
2. task-274-02-darkener-material — bp, M_PP_PlayerLightMask and MPC_PlayerLight as a pure darkener.
3. task-274-03-exposure-raise — bp, PPV_XPGlow exposure raised.
4. task-274-04-darkener-sweep — cpp, -game sweep calibrating the falloff on the DRGS profile, emissives checked.
5. task-274-05-darkener-defaults — bp, chosen values applied.
6. task-274-06-darkener-capture — cpp, final captures with Enable 1 and 0, proof numbers, side by sides.
7. task-274-07-far-floor-sweep — cpp, round 3: floor-only scoring, FarBrightness and Exponent, final side by sides at the chosen values.
8. task-274-08-far-floor-apply — bp, chosen values written to MPC_PlayerLight.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-274-01-mask-writer-cleanup]] — step 1, cpp.
- [[task-274-02-darkener-material]] — step 2, bp.
- [[task-274-03-exposure-raise]] — step 3, bp.
- [[task-274-04-darkener-sweep]] — step 4, cpp.
- [[task-274-05-darkener-defaults]] — step 5, bp.
- [[task-274-06-darkener-capture]] — step 6, cpp.
- [[task-274-07-far-floor-sweep]] — step 7, cpp.
- [[task-274-08-far-floor-apply]] — step 8, bp.
