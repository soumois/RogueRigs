---
id: sys-run-layout
description: "Run layout provider: C++ generator per level from a per-run seed, packaged params and briefs, hand JSON fallback, request points, prepared layout across the travel."
parent: sys-run
kind: doc
---
# Run — layout provider

- responsibility: build the terrain of each run level off the game thread and keep it on the game instance until the grid builds from it. Since [[task-496-generator-in-runs]] every level comes from the C++ generator ([[sys-layout-generator]]) in memory, no file written; the hand layouts are the fallback.
- main_classes (`Source/RogueRigs/Run/RRLayoutProvider.h`): `IRRLayoutProvider` (`BuildLayout(FRRLayoutRequest)` on a worker, thread-safe); `FRRGeneratorLayoutProvider` (default, `URRRunSettings::bGenerateRunLayouts` true): `FRRLayoutGenerator::Generate(LevelSeed, brief)`, layout keeps the level's hand name, `FRRLayoutData::Source` says "generator, run seed, level seed, accepted seed"; `FRRJsonLayoutProvider` (hand `<LayoutName>.json`, the fallback, or the provider when the setting is false). `FRRLayoutRequest`: LevelIndex, LayoutName, RunSeed, LevelSeed, bForceGeneratorFailure.
- seeds: `URRRunSubsystem::PrepareNewRunLayout` draws a random run seed (FGuid hash, 1..999999999) and requests level 1; `BeginRun` keeps a prepared seed, else draws one; `DeriveLevelSeed(run seed, level)` gives the first seed tried per level. Log only (task-496 Q4): no player display, no command to force it.
- data: params and brief per level from `URRRunSettings::LevelGeneratorParams` / `LevelGeneratorBriefs`, in `Content/RogueRigs/Data/Levels` (staged), never `Tools/`: levels 2-4 `arena_lN.params.json` (review section added) + `arena_lN.brief.json`; level 1 `arena_m1_fine.params.json` (copy of `Tools/layout_gen/params/fine_default.json`) + `arena_m1_fine.gen.brief.json` (arena_m1_fine bounds, heal_ore and barrel counts of [[task-491-generator-heal-barrel-plus20]]: the hand brief would reject every seed). Fallback names: `URRRunSettings::LevelLayouts`.
- request points: level 1 when the main menu opens (`ARRMainMenuGameMode::StartPlay`) or at `ResetRun` (Retry, Main Menu, restart) unless a prepared run waits; level N+1 in `AdvanceToNextLevel`, which runs in the frame of the miner's board zone contact (`ARRShuttle::TryBoard` -> `EndRun(true)` -> `HandleLevelExtracted`).
- entry_points (`URRRunSubsystem`, [[sys-run]]): `RequestLayout`, `OnLayoutReady`, `IsLayoutReady` / `IsLayoutPending` / `IsLayoutRequested` / `HasNewRunLayout`, `GetPreparedLayout`, `WaitForPreparedLayout`, `SetLayoutProvider` (null = settings default), `GetRunSeed`, `PrepareNewRunLayout`.
- fallback: params or brief invalid, or no seed passing within `search.max_seeds`: Error line `RRGeneratorLayoutProvider: level N generation failed (...); falling back to the hand layout <name>.json`; the run continues. Debug cvar `rr.Layout.ForceGenFail <level|-1>`.
- threading: `UE::Tasks::Launch`, result handed to the game thread by request serial. A core ticker logs the game-thread frames while a request is pending (`game thread meanwhile: N frame(s), longest X ms`; frame 1, the engine start, not counted). `Deinitialize` waits for an in-flight search (up to about 8 s).
- gotchas: the grid builds level 1 from the prepared layout before or after `BeginRun`; without a preparation (map played directly, PIE, tests) it reads its own `LayoutName`. Generation 0.1 to 8 s (1 to 15 seeds). Checks: [[task-496-01-generator-provider]], [[task-496-02-request-timing]]. No automation test (owner rule).

## Relations
- [[sys-run]] — parent: the run state that holds the provider.
- [[sys-layout-generator]] — the generator it runs.
- [[sys-grid]] — builds from the prepared layout.
- [[sys-run-transition-preload]] — rebuilds the grid in place from it behind the cards screen.
- [[sys-main-menu]] — the menu whose opening requests level 1.
- [[intermission]] — why the next terrain is prepared ahead.
- [[arena-grid]] — the generator rule it implements.
- [[level-layout-schema]] — the hand file format of the fallback.
- [[task-496-generator-in-runs]] — the task that wired the generator in.
- [[task-206-03-transition-flow]] — waits for OnLayoutReady before the map open.
