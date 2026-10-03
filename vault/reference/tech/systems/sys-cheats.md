---
id: sys-cheats
description: "Cheat console variables as built: rr.Cheat.GrantXP, Invulnerable, ChooseUpgrade, Reroll with on-change sinks; open to drive PIE checks through Unreal MCP."
parent: systems-index
kind: doc
---
# Cheat console variables

- responsibility: let a tool that can only set console variables (Unreal MCP, [[unreal-mcp]]) drive PIE checks: grant XP, make the miner invulnerable, pick or reroll the level-up cards. Debug only, no gameplay role. Console commands with arguments stay in [[sys-stress]].
- main_classes: no class; `Source/RogueRigs/Debug/RRCheats.cpp`, namespace `RRCheats`: four `TAutoConsoleVariable` (`ECVF_Cheat`) with a `FConsoleVariableDelegate` sink each. Target world: the first PIE world, else the first game world (headless tests, `-game`). Target miner: player 0's pawn, else the first live `ARRMiner`.
- entry_points (each set fires its sink, even with the same value):
  - `rr.Cheat.GrantXP <N>`: N > 0 calls `URRProgressionComponent::AddXP(N)` on the miner (XPGain applies, [[sys-progression]]), logs level and XP, then resets to 0. Level 1 → 2 needs 5.
  - `rr.Cheat.Invulnerable <0|1>`: 1 adds one loose `State.Invulnerable` count to every miner not yet tagged by it (weapons keep firing, unlike `rr.Stress.Peace`); 0 removes the counts it added. Loose tags are counted, so i-frames and `rr.Stress.Peace` stay independent. A miner spawned later (level restart) is not covered: set 1 again.
  - `rr.Cheat.ChooseUpgrade <i>`: i >= 0 calls `URRLevelUpComponent::ChooseUpgrade(i)` (as a card button, [[sys-level-up]]), logs picked or refused, then resets to -1.
  - `rr.Cheat.Reroll 1`: calls `RequestReroll()` (as the "Relancer" button), logs, then resets to 0.
- data: none.
- depends_on: [[sys-progression]], [[sys-level-up]], [[sys-gas-core]], [[sys-player]].
- implements: none (debug tooling).
- gotchas: the reset uses the priority of the last setter (`SetBy` flags): a lower priority set is refused by the console manager with a warning, so set every cheat from the same source (MCP or console). Cheat variables are compiled out of shipping builds. They work while the game is paused (the screen is open), console variables do not tick. Proven by `RogueRigs.Cheats.ConsoleVariables` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[unreal-mcp]] — the tool that sets these variables during PIE checks.
- [[sys-stress]] — the other debug console tooling (commands).
- [[sys-level-up]] — the screen ChooseUpgrade and Reroll drive.
- [[sys-progression]] — the AddXP GrantXP calls.
- [[sys-automation-tests]] — the test that proves the sinks.
