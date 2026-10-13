---
id: sys-tests-player-cheats
description: "Player damage, i-frames, game over and Cheats tests (console variables, property route, light halo). Open to change player or cheat tests."
parent: sys-automation-tests
kind: doc
---
# Automation tests — player cheats

Part of [[sys-automation-tests]] (harness, command and gotchas live there).

- `RogueRigs.Player.FlatDamage`, `IFrames`, `DeathOnce`: `BP_Miner` on (42, 38), Health set to 100, hit by `URRGE_DamageFlat` from a bare actor with an ASC and `URREnemyAttributeSet`: 10 → 90 and `State.Invulnerable`; `AttackPower` 2 → −20; no enemy set → −10. Same-frame and 0.217 s hits refused, mesh hidden and shown during the window, visible and hit applied after it. 150 → 0: `OnDeath` once (enemy as instigator), `State.Dead`, no i-frames, weapons stopped, move input ignored; a later hit is refused ([[sys-damage]]).
- `RogueRigs.Player.GameOver`: `BP_RRGameMode` CDO `RestartDelay` 3 s; a spawned `BP_RRGameMode` (not the auth game mode: `RegisterMiner` called directly, twice) with `RestartDelay` forced to 4.5 s; the lethal hit fires `OnGameOver` once, `IsGameOver`, restart armed at 4.5 s; after 30 frames and a second `OnDeath` broadcast: still once, timer counting down (29 or 30 frames counted), never reaching the travel.
- `RogueRigs.Cheats.ConsoleVariables` (`BP_Miner` on (42, 38), weapons stopped; [[sys-cheats]]): `rr.Cheat.GrantXP 4` (need(1), task-033-01) → level 2, variable back to 0; 3 more → 3 XP; `ChooseUpgrade 0` refused without a screen (back to -1); after the delay `Reroll 1` → offer shown again, reroll used; `ChooseUpgrade 3` refused, `2` picks card 2 and closes; `Invulnerable 1` twice → one loose `State.Invulnerable` count, `0` → removed.
- `RogueRigs.Cheats.PropertyRoute` (`BP_Miner` on (42, 38), weapons stopped, CDO reset before and after): `GrantXP` 4 (need(1), task-033-01) consumed by one tick → level 2; `bInvulnerable` one loose count over two ticks, cleared → removed; `AlertSpeedMultiplier` 10 → 10 s of alert in 60 frames; `bReroll` → offer shown again; `ChooseUpgrade` 1 → card 1 picked, screen closed; `bSkipToConsole` → console, alert 0.4, pending; `bCompleteConsoleHack` → complete, nothing pending; `bSkipToExtraction` → extraction ready, alert 1; each one-shot field back to neutral.
- `RogueRigs.Cheats.LightHalo` (task-026-04; fine grid, `BP_Miner`, a spawned directional and sky light): off by default, no halo; `rr.Cheat.LightHalo 1` sets `bLightHalo`; next tick a registered point light on the miner `LightHaloHeight` above it, intensity (cd) and radius from the settings, sun and sky × `LightHaloAmbientScale`, camera exposure bias + `LightHaloExposureBias`; an edited intensity applies next tick without compounding the dimming (one light); off restores lights, override flag and bias; CDO reset after ([[sys-cheats]]).

## Relations
- [[sys-automation-tests]] — parent hub: harness, command, gotchas.
- [[sys-damage]] — system exercised by these tests.
- [[sys-cheats]] — system exercised by these tests.
