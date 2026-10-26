---
id: task-317-01-stats-then-cards
description: "The end-of-level stats screen and the cards shop screen are two distinct screens; level 4 shows the stats screen only, then the run-won screen."
parent: task-317-stats-then-cards
kind: task
status: todo
---
# Stats screen then cards screen

- goal: The end-of-level stats screen and the cards shop screen are two distinct screens; level 4 shows the stats screen only, then the run-won screen. Recap kills use robot type names as text (no icons); level 4 stats button leads to the existing run-won screen. No new test (owner rule): only fix existing tests the change breaks.
- agent: cpp
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), run-shop (vault/reference/gdd/run/run-shop.md), run-structure (vault/reference/gdd/run/run-structure.md), sys-run (vault/reference/tech/systems/world/sys-run.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), sys-run-log (vault/reference/tech/systems/world/sys-run-log.md), run-log-schema (vault/reference/tech/pipelines/run-log-schema.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-hud-bindings (vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ in Source/RogueRigs/ (Run, UI): Stats and Shop transition phases, NextLevel(), stats recap data, shop screen widget base and controller class property; updated docs sys-run-transition, sys-run-shop, sys-run
- done_when: build passes; extraction on levels 1-3 opens the stats phase (OnIntermissionStarted, shop not yet shown); NextLevel() opens the shop phase (shop opened, new event OnShopScreenStarted, new controller property ShopScreenWidgetClass with base URRShopScreenWidget carrying GetShop and OnShopRefreshed); Continue only from the shop phase; level 4 extraction opens the stats phase with a run-won flag and its button leads to the existing run-won screen (OnRunEnded(true)), no shop; the recap struct adds kills per robot type (display name and count, from URRRunLogSubsystem KillsByRobotType) and damage dealt per weapon (weapon display name and damage, from DamageDealtByWeapon), read before the level ends; Blueprint API documented; debug commands rr.Run.NextLevel and rr.Run.EndLevel still work (rr.Run.Stats.Next or equivalent documented); existing tests broken by the change updated and passing; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-317-stats-then-cards]] — parent global task.
