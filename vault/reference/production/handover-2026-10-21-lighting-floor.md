---
id: handover-2026-10-21-lighting-floor
description: "State after the drone, light and floor session of 2026-10-20/21: tasks 247 to 274, current look values, unseen items, open owner decisions, quirks. Open first when resuming."
parent: production-index
kind: note
---
# Handover 2026-10-21: drone, lighting, floor

## Owner intent (his words, summarised)
- Drone: patrol in the player zone with spring lag, no flight over walls, real mesh in parts (done).
- Lighting: a FLAT shadowless light plus a post-process shader that is only a DARKENER (gain at most 1, no tint). With the shader off the whole screen must look like the former centre. Later (not now) a light on the player that only touches floor and walls.
- Floor: cracked-plate texture with world-position noise to avoid repetition.
- Reference: DRGS video, ring luminance 164, 140, 84, 57, 39, 30, 22, 17, 12, 10, 9 per 80 px ring; no vignette, no special effect found, textured floor, highlights below about 205/255 ([[drgs-analysis]]).

## Tasks (all committed)
- 247 to 249 drone movement (247 superseded by 251), 250 HealVein test fix, 251 patrol, 252 to 254 drone mesh parts, animation, thrusters: done.
- 255 camera arm 1400, 256 pickup pop 150, 257 pulse desync, 258 heal vibration, 259 ore 2 s, 260 rock 0.65 s: done.
- 261 no shadows, 262 crystal volume, 263 player light (light later removed), 264 blob shadows, 265 and 268 ambience, 266 robot readability, 267 pulse fix, 269 XP bloom, 270 and 271 capture tuning: done.
- 272 mask material, 273 floor texture and material (normal sign verified, no flip): done.
- 274 flat light and darkener: IN PROGRESS, next [[task-274-07-far-floor-sweep]] then 08 (far floor measured 1.8/255, target 9-12).

## Current values
See [[mpp-player-light-mask]], [[lights-dark-ambient]], [[ppv-xp-glow]], [[bp-floor-plates-material]], [[bp-robot-readability]], [[bp-miner]] (revert recipe of the removed PlayerLight). Mask: FarBrightness 0.002, Radius 260, Exponent 4; exposure -3.4 EV100; sun 4.0; sky 0.08.

## Not verified by the owner on screen
Everything above is judged on -game captures (Saved/LookTune) and numbers only.

## Open owner decisions
Rock top albedo (burnt crescent), far floor brightness, XP mesh scale about 0.45, barrels 0.5 or 0.65 s, task-234 deposit look check, the later player light.

## Quirks learned
- Captures: setnopec for light components, macro path with Binaries, r.RecreateRenderStateContext ([[task-270-01-ambience-capture-tuning]]).
- External actors save only through the empty save_assets route; a Save Content dialog blocks CloseMainWindow.
- task-next.js closes multi-subtask globals too early: check statuses.
- API session limits kill parallel agents: run one agent at a time.
- The Unreal MCP server exists only while an editor runs.
- The main session formalises and delegates to the orchestrator agent; the /task command wording says "Act as", which is misleading (fix proposed, not applied).

## Relations
- [[production-index]] — parent index.
- [[tasks-index]] — task status source of truth.
- [[handover-2026-10-18-rock-slab]] — previous handover.
