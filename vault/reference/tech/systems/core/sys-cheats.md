---
id: sys-cheats
description: "Cheats as built: polled URRCheatSettings CDO properties (the route Unreal MCP can drive) and rr.Cheat.* console variables, sharing one set of actions."
parent: systems-core-index
kind: doc
---
# Cheats

- responsibility: drive PIE checks without input: grant XP, invulnerability, pick or reroll level-up cards, alert speed, console and extraction skips. Debug only, no gameplay role. Console commands with arguments stay in [[sys-stress]].
- main_classes (`Source/RogueRigs/Debug/`):
  - `RRCheats.h/.cpp`: namespace `RRCheats` actions shared by both routes: `GrantXP`, `SetInvulnerable`, `ChooseUpgrade`, `Reroll`, `SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction`, `GrantOre` (`URRRunFlowSubsystem::AddCarriedOre`, [[sys-objectives-ore-counter]]), plus `FindCheatWorld`, `FindPlayerMiner` (player 0's pawn, else the first live `ARRMiner`). Every log line starts with `rr.Cheat.<Name> (<route>)`.
  - `URRCheatSettings` (`RRCheatSettings.h`, UObject): cheat fields on the class default object `/Script/RogueRigs.Default__RRCheatSettings`.
  - `URRCheatSubsystem` (tickable world subsystem, Game/PIE, ticks while paused, absent in shipping): every frame reads the CDO, applies to its own world and resets one-shot fields.
  - gameplay entry points the actions call (Blueprint category `Cheats`, `//~ Begin cheats` header section, bodies compiled out of shipping: no effect, null or false): `URRRunFlowSubsystem::SetAlert`, `SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction`; `ARRConsole::ForceComplete` (normal hack completion uses protected `CompleteHack`) ([[sys-objectives]]).
- property route (use this from Unreal MCP: `ObjectTools.set_properties` on the CDO path during PIE; [[unreal-mcp]]):
  - one-shot, consumed by the next frame and reset: `GrantXP` (float > 0, reset 0), `GrantOre` (int32 > 0: N carried ore, reset 0; refused with a warning once the run ended or the miner died), `ChooseUpgrade` (0..2, reset -1), `bReroll`, `bSkipToConsole` (alert jumps to the threshold, console drops), `bCompleteConsoleHack`, `bSkipToExtraction` (console completed or skipped, alert full, `OnExtractionReady`) (bools reset false). Read a field back: neutral = consumed.
  - light halo (task-026-04, look test, persistent, category Cheats|LightHalo): `bLightHalo` (default false) adds a transient, movable point light `RRCheatLightHalo` on the player miner and dims the arena: every non-static directional and sky light × `LightHaloAmbientScale` (0.3) and the miner camera `AutoExposureBias` + `LightHaloExposureBias` (-1; auto exposure would brighten a dimmed scene back). Light values: `LightHaloIntensity` 60 cd, `LightHaloRadius` 900 uu, `LightHaloHeight` 250 uu, `LightHaloColor` (1, 0.85, 0.65), `bLightHaloShadows` false. Editable while on (applied next frame, dimming not compounded); off, a new world or the subsystem ending restores lights and camera. `URRCheatSubsystem::GetLightHalo` returns the light. Values provisional (owner look test).
- persistent: `bInvulnerable` (applied on change; true with no miner yet is retried; applied again in each new world, so it survives a level restart), `AlertSpeedMultiplier` (1; multiplies alert gain from time and kills, read directly by the run flow, [[sys-objectives]]), `ShuttleCountdownOverride` (0; seconds > 0 replace the shuttle countdown at its next landing, for a quick run-lost check; property route only). The CDO lives as long as the editor: set them back after a check.
- console variable route (`ECVF_Cheat`, sinks fire on set, act on the first PIE world else the first game world): `rr.Cheat.GrantXP <N>` (reset 0), `rr.Cheat.GrantOre <N>` (int, N carried ore, reset 0; log `rr.Cheat.GrantOre (<route>): +N ore, carried C, secured S.`), `rr.Cheat.Invulnerable <0|1>`, `rr.Cheat.ChooseUpgrade <i>` (reset -1), `rr.Cheat.Reroll 1`, `rr.Cheat.AlertSpeed <x>` (writes `AlertSpeedMultiplier`), `rr.Cheat.SkipToConsole 1`, `rr.Cheat.CompleteHack 1`, `rr.Cheat.SkipToExtraction 1` (reset 0), `rr.Cheat.LightHalo <0|1>` (writes `bLightHalo`; owner: type it in the PIE or game console). Visual check commands (`RRVisualCheckCommands.cpp`, not shipping): `rr.Debug.Shots <Delay> <Count> <Interval> [Prefix] [ShowUI]` (ShowUI 1 captures the window with the HUD), `rr.Debug.Delayed <Seconds> <Command...>` (runs a command later through player 0: chains `-ExecCmds` steps, comma-separated), `rr.Debug.DamageMiner <Amount>` (lowers Health, min 1: a hit for the vignette; logs vignette opacity, HitFlash and frame delta for 6 frames, and `rr.Debug.Shots` logs them at each shot, task-055-01), `rr.Debug.SpawnNear <Class> <Distance> [Yaw] [Count]` (task-027-03: actors of a /Game class path or a robot name around the player miner; a kamikaze at 140 uu flares at once), `rr.Debug.Objective <Console|Shuttle> [Distance]` (miner moved 150 uu from the console with its marked rocks destroyed so the drone hacks, or 900 uu from the shuttle; logs the bearing to the target). For the in-game console or `-ExecCmds`; Unreal MCP cannot set them.
- invulnerable rule: one loose `State.Invulnerable` count per miner, shared list for both routes (counted tags keep i-frames and `rr.Stress.Peace` independent); clearing either route removes every count the cheat added.
- data: none.
- depends_on: [[sys-progression]], [[sys-level-up]], [[sys-gas-core]], [[sys-player]], [[sys-objectives]].
- implements: none (debug tooling).
- gotchas: a CVar reset uses the priority of its last setter (`SetBy`); a lower-priority set is refused with a warning. One-shot properties are consumed by whichever game world ticks first. Proven by `RogueRigs.Cheats.ConsoleVariables` and `RogueRigs.Cheats.PropertyRoute` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[unreal-mcp]] — the tool that sets the properties during PIE checks.
- [[sys-stress]] — the other debug console tooling (commands).
- [[sys-level-up]] — the screen ChooseUpgrade and Reroll drive.
- [[sys-progression]] — the AddXP GrantXP calls.
- [[sys-objectives]] — the run flow the alert and console cheats drive.
- [[sys-automation-tests]] — the tests that prove both routes.
- [[sys-objectives-ore-counter]] — the ore counter GrantOre feeds.
- [[sys-progression]] — the rr.Pacing.Run XP pacing bot (Debug folder) and its results.
