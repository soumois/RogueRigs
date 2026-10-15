---
id: task-183-01-ore-counter-logic
description: "C++ ore counter: carried count from ore pickups, secured on extraction in the game instance, emptied on a miss or death, HUD event, GrantOre cheat."
parent: task-183-ore-counter
kind: task
status: done
---
# Ore counter logic (C++)

- goal: C++ owns the ore counts. Collecting an ore pickup adds its `OreValue` to the carried count. `ARRShuttle` extraction success adds the carried count to a secured total held by a game-instance subsystem (survives `RestartLevel` for the session), then sets carried to 0. A missed shuttle or the miner's death sets carried to 0 (secured unchanged). A HUD panel event (pattern of [[sys-hud-events]]) sends carried and secured on every change and once at bind time; getters for both. Cheat `GrantOre` on both routes of [[sys-cheats]] (`rr.Cheat.GrantOre <N>` CVar and a one-shot CDO property), adding N carried ore. The share of secured ore lost on a failed run is `open`: not built.
- agent: cpp
- depends_on: none
- inputs: ore-economy (vault/reference/gdd/run/ore-economy.md), extraction (vault/reference/gdd/run/extraction.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-objectives (vault/reference/tech/systems/world/sys-objectives.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in `Source/RogueRigs/` (class names and placement are the agent's choice, following existing conventions: events over polling); the docs above updated where the system changed (ore counter section in sys-pickups or sys-objectives, the event in sys-hud-events, the binding in sys-hud-bindings, the cheat in sys-cheats).
- done_when: editor build succeeds with no new warning; existing automation tests in the touched areas (Cheats, Extraction, Pickups, HUD) pass, and only expectations a change breaks are updated, no new test; a `-game` or headless run log shows `rr.Cheat.GrantOre 10` raising carried to 10; docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 84, cache write 125974, cache read 3634013, output 4030
- runs: 1
- milestone: M1
- result: Carried ore in URRRunFlowSubsystem (AddCarriedOre, OnOreChanged(Carried, Secured), getters), secured ore in new URROreBankSubsystem (game instance), emptied on missed shuttle or miner death, GrantOre cheat both routes; build exit 0, 38/38 tests pass, -game log shows GrantOre 10 -> carried 10; commits 7a452036, cb0916e5

## Relations
- [[task-183-ore-counter]] — parent global task.
