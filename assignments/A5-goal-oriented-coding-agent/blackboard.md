# Code Architect — blackboard

Every decision of the Code Architect (`/architect`, [source](../../.claude/commands/architect.md)) is written here **before** it acts on it, and committed section by section (`architect run N: <section>`). Past sections are never rewritten. The current state is in [state.md](state.md).

Sections of a run:
1. **Base:** commit the run starts from.
2. **Scan:** every GDD feature, `built` / `partial` / `missing`, with evidence.
3. **Scoring:** missing features ranked, score and reason per factor; excluded features and why.
4. **Recommendation:** the top buildable feature, then the owner's answer, quoted verbatim.
5. **Plan:** global task and subtasks.
6. **Build:** per subtask, the exact prompt sent to the subagent, then files, build and test result, commit.
7. **Rescan:** the feature is now `built`; final agent commit.
8. **Owner review:** what the owner changed, in a separate commit.

<!-- Runs are appended below by the agent. -->

## Run 1 — 2026-10-14

### 1. Base
- Base commit: `468e91a2` (`task-182: debris 4.5 s full size + 0.4 s sink (owner)`).
- Owner constraint for this run: none.
- Resume (orchestration-protocol step 0): no global task `in-progress`. One open task in tasks-to-do-index: `task-182-debris-pool-80-test` (status `todo`, prepared, not started). Not touched by this run.
- Working tree not clean at start (owner's uncommitted vault, art and `WBP_UpgradeChoice.uasset` edits): this run commits only the files it writes, by name.
- state.md: first run (nothing built, failed or decided).

### 2. Scan
Method: 38 `kind: spec` nodes under `gdd-index` (all `approved` or `provisional`; none `deferred` at node level). Four read-only scan agents (run; player and weapons; enemies and terrain; art, audio and lore), one per branch, each told to verify every evidence path. The Architect re-checked the deciding gaps itself (`grep` in `Source/RogueRigs`: no ore counter symbol, no `Threat` symbol, no `URRGE_EnemyDifficulty`, `ERRUpgradeCardType` = `StatUpgrade`, `NewWeapon` only). Paths are relative to `Source/RogueRigs/` unless they start with `Content/`, `Tools/` or `vault/`. D = `Content/RogueRigs/Data/Tables/`.

#### Run
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| difficulty: alert grows with time and kills, pauses on objective | M1 | built | `Objectives/RRRunFlowSubsystem.h` `GetAlert`, `NotifyEnemyKilled`, `HasPendingObjective` |
| difficulty: hidden threat counter | M1/M2 | missing | no `Threat` symbol; `vault/reference/tech/systems/core/sys-difficulty.md` "not built" |
| difficulty: threat scales robot health and attack at spawn | M2 | missing | `URRGE_EnemyDifficulty` named in sys-difficulty but absent from Source (doc drift) |
| difficulty: threat raises spawn rate | M2 | missing | `Enemies/RRWaveDirector.cpp` `MakeDefault` ramps on time only |
| difficulty: denser and harder between levels | M2 | partial | `FRRWaveDirectorConfig::MakeLastLevel` exists, never called |
| difficulty: overtime from 3:30 | M1 | built | `Enemies/RRWaveDirector.h` `FRRDirectorOvertime` |
| level-flow: M1 fixed sequence (240 s, console at 40 %, extraction when full) | M1 | built | `Objectives/RRRunFlowConfig.h` `ConsoleAlertThreshold`; `URRRunFlowSubsystem::CheckExtraction` |
| level-flow: 4–7 tiers drawn from an event pool | M2 | missing | only the fixed sequence (sys-objectives "no tier pool") |
| level-flow: horde banner and alert-bar icons | M1 | built | `URRRunFlowSubsystem::OnHordeIncoming`, `ComputeAlertBarEvents` |
| level-flow: music per tier | M3 | missing | no audio code |
| objective-console: drop, footprint, marked rocks, hack, pause | M1 | built | `Objectives/RRConsole.h` `MarkedCellTypes`, `HackDuration`; `URRRunFlowSubsystem::DropConsole` |
| objective-console: locator arrow with distance in metres | M1 | partial | `UI/RRHUDWidget.h` `GetObjectiveLocator`; `FRRObjectiveLocator` (`UI/RRHUDTypes.h`) has no distance field |
| objective-bonus-chest: drop, hack, XP and luck-card reward | M2 | built | `Objectives/RRBonusChest.h` `GetXPNeedFraction`, `GetBonusLuck` |
| objective-bonus-chest: 5 ore chunks reward | M2 | partial | `GetOreChunkCount` drops chunks; no ore counter credits them |
| objective-variants: special ore objective | M2 | partial (stub) | `Grid/RRArenaGrid.h` `ERRCellType::SpecialOre` only |
| ore-economy: ore chunks drop, worth 1, use PickupRadius | M1 | built | `Pickups/RRPickup.h` `OreValue`; D`OreTypes.csv` `ChunkValue` |
| ore-economy: carried ore counted, secured on extraction, lost on a miss | M1 | missing | `Pickups/RRPickup.cpp:342` only calls `ARRMiner::NotifyPickupCollected` (a broadcast); no ore total anywhere in Source |
| ore-economy: share of secured ore lost on a failed run | M2 | missing | value `open` |
| extraction: shuttle landing, 45 s countdown, win or lose, screens | M1 | built | `Objectives/RRShuttle.h` `OnExtracted`, `OnMissed`; `URRRunFlowSubsystem::LandShuttle`, `EndRun` |
| extraction: ore secured on success, lost on a miss | M1 | missing | same gap as ore-economy |
| run-structure: 4 levels back to back, carry-over, win at level 4 | M2 | missing | `RestartLevel` reopens the same map; no level index or persistence |
| run-structure: lose on death or missed shuttle | M1 | built (one level) | `Player/RRGameMode.h` `OnGameOver` |
| xp-level-up: drops, cubes, big chip, curve, level-up sequence, HUD bar | M1 | built | D`XPCurve.csv`; `Progression/RRLevelUpComponent.h`; `Pickups/RRPickupSubsystem.h` `FlushXPCubes` |
| xp-level-up: level kept across the 4 levels | M2 | missing | no persistence |

#### Player and weapons
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| drone: orbit, arc dig, fallback, dig times, shake, hack | M1 | built | `Drone/RRDrone.cpp` `TickOrbit`, `SelectArcCells`, `RefreshDigShake`, `StartHack` |
| drone: dialogue goes through the drone | M1 | missing | `Text/RRSubtitleSubsystem.h` has no drone link; barks never shown |
| drone: laser colour, final look | M1 | partial | grey box; laser colour `open` |
| miner: move, aim, auto-fire, turret button, no shot through rock, i-frames, heal rules, death | M1 | built | `Player/RRPlayerController.cpp`, `Weapons/RRAutoFireComponent.cpp`, `Player/RRMiner.h` `IFramesDuration`, `Player/RRGameMode.h` |
| miner: plate-armour mesh | M1 | missing | BP_Miner uses SKM_Quinn_Simple |
| player-stats: six stats, base values and effects | M1 | built | D`PlayerBaseStats.csv`; `GAS/RRPlayerAttributeSet.h` |
| player-stats: stat sheet UI (own stats + summed global bonuses) | M1 | missing | only a comment at `Upgrades/RRUpgradeComponent.h:110` |
| burst-rifle: burst fire, stats, crit, pierce, projectile sizes | M1 | built | D`WeaponStats.csv` `BurstRifle`; `Damage/RRDamageExecution.cpp` |
| rifle, shotgun, turret: magazine upgrade | after M1 | missing | `Upgrades/RRUpgradeCard.h:35` "Not offered in M1: no row"; no values in the spec |
| burst-rifle: 4-direction fire effect | M2 | missing | `ERRUpgradeCardType` has no effect type |
| rifle, shotgun, turret: name, muzzle, impact, sound | M1/M2 | partial | grey box; values `open` |
| damage-numbers: one per hit, crit, stacking, Niagara atlas | M2 | built | `Damage/RRDamageNumberSubsystem.h` `AddHit`, `CriticalScale`, `StackDirection` |
| damage-numbers: player can turn them off | M2 | partial | `Settings/RRGameUserSettings.h` `SetShowDamageNumbers`, no caller |
| rarities: five tiers, luck draw, UI colours, chest luck | M1 | built | `Upgrades/RRUpgradeCard.h` `ERRUpgradeRarity`; `Upgrades/RRUpgradeComponent.cpp` `GetTierWeights` |
| shotgun: fan, pellets, knockback, magazine, weapon card | M1 | built | `Weapons/RRShotgun.cpp` `GetPelletYawOffset`; `URRUpgradeComponent::MakeWeaponCard` |
| turret: placement, charges, spacing, lifetime, lock, stats, size | M2 | built | `Weapons/RRTurretLauncher.cpp` `TryPlaceTurret`; `Weapons/RRTurret.cpp` `InitFromLauncher` |
| upgrades: 3 cards, reroll, targeted vs global, stats, pierce, weights, weapon cards, stacking | M1/M2 | built | `URRUpgradeComponent::BuildOfferWithOwnerLuck`, `ReplayGlobalUpgrades`; D`UpgradeValues.csv` |
| upgrades: effect cards (4-direction fire) | M2 | missing | no effect card type |
| upgrades: EMP | M3 if time | missing | no status effects |

#### Enemies and terrain
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| enemy-chaser: state machine, stats, size, shield variant, shield mix | M1 | built | `Enemies/RRChaser.cpp` `UpdateBehavior`; `Enemies/RRShieldChaser.cpp`; D`EnemyStats.csv` |
| enemy-chaser: Meshy rigid parts and baked clips | M1 | partial | `ARRChaser::PlayRunClip`; triangle budget, foot slide and perf still open (handover-2026-10-14) |
| enemy-death: shrink, puff, debris draw, throw, pool caps | M1/M3 | built | `Enemies/RREnemy.cpp` `HandleDeath`, `LaunchDebris`; `Enemies/RRDebrisPiece.cpp` |
| enemy-death: parts for heavy, kamikaze, shooter | M3 | missing | parts `open` |
| enemy-heavy: state machine, slam, stats, capsule, ground signal | M3 | built | `Enemies/RRHeavy.cpp` `PerformSlam`, `GroundTelegraph` |
| enemy-heavy: at most 3 alive | M3 | partial | Blueprint only; `Enemies/RREnemySpawner.h:606` and sys-spawner still say 5 (doc drift) |
| enemy-kamikaze: chase, arm, flare, explode, kill rules, stats, spawn share | M1 | built | `Enemies/RRKamikaze.cpp` `Detonate`, `HandleDeath` |
| enemy-shooter: hold, line of sight, reposition, laser, on-screen fire, shots | M2 | built | `Enemies/RRShooter.cpp` `UpdateLaserBeam`, `IsOnScreen` |
| enemy-shooter: hold tolerance ±1 cell | M2 | partial | CSV tolerance 200 uu (about 3 cells, predates the 67 uu grid) |
| arena-grid: grid, JSON layout, 240 × 240 × 67 uu, passages, flow field | M1 | built | `Grid/RRArenaGrid.h` `CellSize`, `LoadLayout`; `FlowField/RRFlowFieldSubsystem.h` |
| arena-grid: autotiling and top-face variants | M1/M3 | partial | sys-grid "not implemented"; HISM instancing built |
| arena-grid: stars and planet backdrop | M1 | missing | no sky assets |
| arena-grid: deterministic C++ generator at runtime | M2 | missing | only offline `Tools/layout_gen/arena_fine.js` |
| barrel: health, fuse, chains, ground zone, mesh | M1 | built | `Grid/RRArenaGrid.h` `BarrelFuseSeconds`, `GetBarrelTelegraph` |
| barrel: particle burst on destruction | M1 | missing | nothing in `Grid/RRArenaGrid.cpp`; sys-grid "not implemented" |
| destruction-rules: shots never break blocks, 3 breakers | M1 | built | `Grid/RRArenaGrid.h` `ERRTerrainDamageSource` |
| destruction-rules: destroyed blocks logged by source and type | M2 | partial | `OnCellDestroyed(Cell, Type, Source)` delegate; sys-logging not written |
| explosions: terrain, no one-shot, chain delay, grey-box sphere | M1 | built | `Explosions/RRExplosionSubsystem.cpp` `QueueExplosion`, `MinerHealthLeftAtFullHealth` |
| heal-ore: block, pickups, refusal at full health, vein dig | M1/M2 | built | `Pickups/RRPickup.cpp` `IsRefusedByMiner`; D`OreTypes.csv` `Heal` |
| ore-blocks: dig time, deposits, 8 % Duskite, explosions, marking | M1 | built | `Grid/RRArenaGrid.h` `BuildDeposits`, `PlaceOreDeposits` |
| ore-blocks: chunk lands within one cell, never on rock | M1 | partial | `Pickups/RRPickup.cpp` `StartLife` pop 80–220 uu, no rock check |

#### Art, audio, voice
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| color-code: blocks, player side, marked rocks, barrel sign | M1 | built | `Content/RogueRigs/Materials/GreyBox/MI_GB_*` |
| color-code: big XP chip colours, robot logo plates, crit number white-gold | M1/M2 | partial | `CriticalColor` (1, 0.55, 0.08) is orange-gold; chip core lilac vs magenta trail |
| ground-telegraph: decal, modes, uses, hide rules | M1 | built | `Telegraph/RRGroundTelegraphComponent.h` |
| ground-telegraph: fill opacity per mode | M2 | partial | material default only |
| visual-style: grey box per block type | M1 | built | `M_GreyBox` + `MI_GB_*` |
| visual-style: near-vertical camera | M1 | partial | `Player/RRMiner.h` `CameraPitch` = -50 |
| audio-direction: SFX and music | M3 | missing | no sound code or assets |
| company-voice: Kessler-Voss alert subtitles | M2/M3 | built | `Text/RRSubtitleSubsystem.h` `ShowAlertLine`; `Content/RogueRigs/Data/Text/alerts.json` |
| company-voice: tier texts, announcements, loading tips | — | missing | unscheduled |
| crew-characters, fictional-films: barks shown through the drone | M3 | partial | `Content/RogueRigs/Data/Text/barks.json`; `URRTextContentSubsystem::GetBark` has no caller |
| kessler-voss-logo; lore | — | built; n/a | logo on the barrel sticker; lore is a reference node |

Summary: the M1 core loop is built. Logic gaps: ore counter, threat, 4-level run, tier pool, effect cards, stat sheet, damage-number toggle, drone dialogue channel, logging. The rest is art or audio.

### 3. Scoring
Factors: **MS** milestone (M1 = 3, M2 = 2, M3 = 1; a sub-feature whose content is listed in a later milestone in `milestones` takes that one); **P** pillar fit (0–2, P1 gamefeel, P2 manual aim, P3 arena as weapon; 0 = passes no veto test); **Dep** dependencies ready (no = total 0, "blocked by X"); **U** number of other gaps it closes or unblocks; **C** cost (+1 `partial`, 0 `missing`); **Open** a needed value is `open` or not given = not buildable, reported. Total = MS + P + U + C.

#### Buildable
| Feature (spec) | MS | P | Dep | U | C | Total | Reason |
|---|---|---|---|---|---|---|---|
| Ore counter: carried ore counted, secured by extraction, lost on a miss (ore-economy, extraction) | 3 | 1 | yes | 2 | 0 | **6** | Approved M1 rule in two specs and the stake of the risk window (extraction: "This gives the risk window its stake"). Pickups (`ARRPickup::OreValue`), `ARRShuttle::OnExtracted` / `OnMissed` and the HUD event pattern exist. Closes the extraction ore rule and the bonus-chest ore reward; prerequisite for the open ore-loss share and run carry-over. P1: a count the player watches grow and can lose. |
| Locator distance in metres (objective-console) | 3 | 1 | yes | 0 | 1 | 5 | Small; `FRRObjectiveLocator` lacks a distance field, Blueprint may already compute it (unverified). |
| Barrel particle burst on destruction (barrel) | 3 | 2 | yes | 0 | 0 | 5 | P1 + P3, but mostly a Niagara asset (bp), not a code gap. |
| Deterministic C++ arena generator (arena-grid) | 2 | 2 | yes | 1 | 0 | 5 | Unblocks the 4-level run, but by far the largest item (port of `Tools/layout_gen`), several sessions. |
| Drone as the dialogue channel, barks shown (drone, crew-characters, fictional-films) | 1 | 1 | yes | 2 | 0 | 4 | Drone spec says M1 but barks are M3 in `milestones`; 15 triggers to wire to game events. |
| Stat sheet UI (player-stats) | 3 | 0 | yes | 0 | 0 | 3 | Passes no pillar test (pure information); widget-heavy. |
| Damage-number on/off (damage-numbers) | 2 | 0 | yes | 0 | 1 | 3 | Setter exists; no options menu to host it. |
| Destroyed-block run log (destruction-rules) | 2 | 0 | yes | 0 | 1 | 3 | Delegate exists; sys-logging not designed yet. |
| Shooter hold tolerance ±1 cell (enemy-shooter) | 2 | 0 | yes | 0 | 1 | 3 | One CSV value; tuning, owner's feel. |

#### Excluded
| Feature | Reason |
|---|---|
| Threat counter (difficulty) | `open`: threat gain per second and per kill. |
| Threat scales health, attack and spawn rate (difficulty) | blocked by the threat counter; bonus per step `open`. Doc drift reported: sys-difficulty names `URRGE_EnemyDifficulty`, which does not exist. |
| Harder levels, level carry-over, win at level 4 (difficulty, run-structure, xp-level-up) | blocked by the 4-level run, itself blocked by the C++ generator or extra maps; objective per level and the ramp are `open`. |
| Alert tier event pool (level-flow) | `open`: tier count, draw weights and conditions, swarm size. |
| Special ore objective (objective-variants) | `open`: X, vein look, fit with tiers. |
| Effect cards / 4-direction fire (upgrades, burst-rifle) | value not given: strength of the weaker shots. |
| Magazine-size upgrade (upgrades) | values not given in the spec. |
| EMP (upgrades) | M3 "if time"; blocked by status effects (not built). |
| Ore lost on a failed run (ore-economy) | `open`: loss share. |
| Audio and music per tier (audio-direction, level-flow) | blocked by sound assets; palette and mix rules `open`. |
| Robot parts for heavy, kamikaze, shooter; chaser Meshy items (enemy-*) | parts `open`; chaser items wait on the owner (handover-2026-10-14). |
| Miner mesh, space backdrop, autotiling, logo plates, weapon names and sounds | art assets, or names `open`: not code. |
| Camera near-vertical; ore chunk within one cell | no numeric target ("owner to judge"): `open`. |
| Critical number colour | conflict, not a gap: spec says white-gold, code comment says orange-gold was owner-confirmed. Reported. |
| Heavy cap 3 vs 5 | doc drift (`RREnemySpawner.h:606`, sys-spawner), not a feature. Reported. |
| Company tier texts, announcements, tips | unscheduled in `milestones`. |
| Boss, meta shop, elements, armor | wishlist / deferred. |

### 4. Recommendation
- **Feature:** ore counter — carried ore counted during the level, secured by a successful extraction, lost on a missed shuttle (and on death).
- **Why:** highest score (6). Approved M1 rule stated in two specs; without it the extraction risk window has no stake and the bonus chest's 5 ore chunks count for nothing. Every dependency exists (ore pickups with `OreValue`, shuttle `OnExtracted` / `OnMissed`, HUD event widgets). Small: one C++ component or subsystem, one HUD event, one cheat, one small widget.
- **GDD nodes:** `ore-economy` (vault/reference/gdd/run/ore-economy.md), `extraction` (vault/reference/gdd/run/extraction.md).
- **`open` values:** the share of secured ore lost on a failed run stays `open` and is NOT built. Questions put to the owner, each with a default:
  1. Where secured ore lives in M1 (single level, `RestartLevel` reloads the map): default = kept in the game instance for the session, shown on the success screen.
  2. Death also loses carried ore (the spec only says "missing the shuttle"): default = yes.
  3. HUD: one new widget for the carried count (owner rule: one widget per logic in `WBP_HUD`): default = yes, placement next to the XP bar, violet Duskite colour.
- **One-click test (sys-cheats):** new cheat `rr.Cheat.GrantOre <N>` (CVar and CDO property route, like `GrantXP`): adds N carried ore; then `rr.Cheat.SkipToExtraction 1` and board the shuttle → success screen shows N secured; or with `ShuttleCountdownOverride` let it expire → carried ore drops to 0.
- **Status:** stopped, waiting for the owner's answer.

#### Owner answer (verbatim)
> Ok, juste pour l'info, tu peux la mettre à coté du radar. Pour cela, il te faudra aussi l'icone et demande au passage un rendu visuel proposé par l'agent Art in game ( il en aura besoin pour créer l'icone HUD )
>
> Dans Blackboard, traduit chaque fois en anglais

English translation: "Ok. Just so you know, you can put it next to the radar. For that you will also need the icon, and while you are at it ask the Art agent for a proposed in-game visual render (it will need it to create the HUD icon). In the blackboard, translate into English every time."

Decision: **approved**: ore counter. "Ok" accepts every default: (1) secured ore kept in the game instance for the session and shown on the success screen; (2) death also loses carried ore; (3) one dedicated widget, placed **next to the radar (minimap)** instead of the XP bar. Added by the owner: an ore icon for the HUD, preceded by an in-game visual render proposed by the Art agent. Rule for the blackboard: owner quotes are kept verbatim and followed by an English translation.

### 5. Plan
- GDD check (orchestration-protocol step 2): `ore-economy` and `extraction` (approved, M1). No conflict with the GDD. `open` and not built: share of secured ore lost on a failed run. Milestone M1. Placement: `WBP_HUD` puts the minimap disc at x 20–220, y 20–220 and the health bar at y 232; the space right of the disc is free up to the alert bar (x 660).
- Correction to the scan: `wbp-minimap-locator` shows the locator already displays the distance in metres (Blueprint `UpdateLocatorView`), so that row is `built`, not `partial`. The ranking does not change.
- Global task: [`task-183-ore-counter`](../../vault/reference/tasks/task-183-ore-counter.md), milestone M1, one owner point, four subtasks because it spans C++, art and Blueprint:

| # | Subtask | Agent | Depends on | Output |
|---|---|---|---|---|
| 1 | [`task-183-01-ore-counter-logic`](../../vault/reference/tasks/task-183-ore-counter/task-183-01-ore-counter-logic.md) | cpp | — | carried and secured counts, HUD event, `rr.Cheat.GrantOre`, docs |
| 2 | [`task-183-02-ore-hud-render`](../../vault/reference/tasks/task-183-ore-counter/task-183-02-ore-hud-render.md) | art | — | ≤ 2 in-game renders of the counter next to the radar; owner picks |
| 3 | [`task-183-03-ore-hud-icon`](../../vault/reference/tasks/task-183-ore-counter/task-183-03-ore-hud-icon.md) | art | 2 + owner choice | ≤ 2 HUD ore icons; owner picks |
| 4 | [`task-183-04-ore-counter-widget`](../../vault/reference/tasks/task-183-ore-counter/task-183-04-ore-counter-widget.md) | bp | 1, 3 | `WBP_OreCounter` next to the radar, secured ore on the success screen, docs |

- Order: 1 and 2 can run in parallel (C++ with the editor closed; art needs no editor). 3 waits for the owner's pick of a render, 4 for the owner's pick of an icon.
- Not planned (owner rules): no new automation test, no bot run; ore loss on failed run (`open`).
- Status: plan shown to the owner, waiting for confirmation (protocol step 4).

#### Owner confirmation (verbatim)
> Oui tu peux lancer toutes les tâches

English translation: "Yes, you can launch all the tasks."

Decision: plan confirmed; task-183 set `in-progress`. The whole plan runs without asking again, except the two image choices that only the owner can make (render, then icon: art definition of done).

### 6. Build

#### task-183-01-ore-counter-logic (cpp) — prompt sent
Reading list: the subtask node, then its `inputs` (ore-economy, extraction, sys-pickups, sys-objectives, sys-hud-events, sys-hud-bindings, sys-cheats, sys-player, sys-automation-tests, agent-cpp, build-toolchain), resolved paths in the node.
```text
Subtask task-183-01-ore-counter-logic (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-183-ore-counter/task-183-01-ore-counter-logic.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Editor state: closed (checked: no UnrealEditor process). Build from the command line per build-toolchain. If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Git: prefix every git command with `git -c "safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs"`. The working tree holds the owner's unrelated uncommitted changes (vault, Art, WBP_UpgradeChoice.uasset): commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`.
- Scope: exactly the subtask goal. No new automation test (owner rule); only update expectations of existing tests that your change breaks. Do not build the `open` ore-loss share.
- Design constraints: events over polling; follow the existing HUD panel event pattern (sys-hud-events, URRHUDPanelWidget) so a Blueprint panel can bind it in task-183-04; the cheat follows sys-cheats (both routes, shared RRCheats action, compiled out of shipping like the others).
- Do not modify the task node fields status, model, tokens, runs, result.
- Commit message: `task-183-01-ore-counter-logic: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Report: files changed, classes and public API added (exact names of the HUD event/delegate and its payload, getters, the cheat property and CVar), build result, test results (names, pass/fail), the run log line proving `rr.Cheat.GrantOre 10` raises carried ore to 10, docs updated, commit hash(es), what is unverified.
```

#### task-183-02-ore-hud-render (art) — prompt sent
Reading list: the subtask node, then its `inputs` (agent-art, art-3d-lessons, art-style-guide, color-code, readability-rules, kie-image-workflow, ore-blocks, wbp-hud, wbp-minimap-locator), plus `Art/Source/stat-icons/` and a screenshot under `Saved/Screenshots/` if any.
```text
Subtask task-183-02-ore-hud-render (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-183-ore-counter/task-183-02-ore-hud-render.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- The owner asked (2026-10-14, translated): "put the ore counter next to the radar; you will need the icon too, and ask the Art agent for a proposed in-game visual render — it will need it to create the HUD icon." This subtask is the render only; the icon is the next subtask.
- What to show: a gameplay frame of Rogue Rigs (top-down, grey-box arena per visual-style and color-code) with the HUD top-left: round radar disc, health bar under it, and the new ore counter to the right of the disc: a small Duskite ore icon (violet crystal, color-code) and a carried count such as "27". Keep other HUD elements as wbp-hud describes them. Use image-to-image with the stat icons (Art/Source/stat-icons/) and, if one exists, an in-game screenshot (Saved/Screenshots/) as references.
- Limits: at most 2 candidates; one kie_image.js run at a time, never in parallel; never write the API key anywhere; never put an image in vault/assets/.
- Editor: not needed.
- Git: prefix every git command with `git -c "safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs"`. Do not commit the candidate images (only the retained one is committed after the owner's choice); you may commit the prompt file and run log by explicit path with message `task-183-02-ore-hud-render: <summary>` ending with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. Never `git add -A`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: the paths of the candidate PNGs, one line per candidate describing the icon and the counter, your self-review per image against the art-style-guide A rules, credits used.
```

Both delegated in parallel (no shared files: C++ with the editor closed; art needs no editor).

#### task-183-01-ore-counter-logic (cpp) — result
- Files: `Source/RogueRigs/Objectives/RROreBankSubsystem.h/.cpp` (new), `Objectives/RRRunFlowSubsystem.h/.cpp`, `Pickups/RRPickup.h/.cpp`, `Debug/RRCheats.h/.cpp`, `Debug/RRCheatSettings.h`, `Debug/RRCheatSubsystem.cpp`; docs: new `sys-objectives-ore-counter`, updated `sys-objectives`, `sys-pickups`, `sys-hud-events`, `sys-hud-bindings`, `sys-cheats`.
- Design: carried ore in `URRRunFlowSubsystem` (`AddCarriedOre`, `GetCarriedOre`, `GetSecuredOre`, delegate `OnOreChanged(CarriedOre, SecuredOre)`); secured ore in the new game-instance subsystem `URROreBankSubsystem` (survives `RestartLevel`, not saved to disk). `EndRun(true)` secures, `EndRun(false)` and the miner's `OnDeath` empty the carried count. Cheat `RRCheats::GrantOre`, CVar `rr.Cheat.GrantOre <N>`, CDO property `URRCheatSettings::GrantOre`.
- Build: exit 0 (editor closed). First build failed: a missing `#include "Weapons/RRWeapon.h"` hidden by the unity build surfaced once the files left it; fixed in the two .cpp files (root cause in `Upgrades/RRUpgradeCard.h`, reported, not changed).
- Tests: 38/38 pass (Cheats, Extraction, Pickups, PickupPool, HUD, RunFlow); no new test, no expectation changed.
- Run: `-game -nullrhi` log `rr.Cheat.GrantOre (rr.Cheat): +10 ore, carried 10, secured 0.`
- Unverified at runtime: real pickups, securing on extraction, loss on miss or death, survival across restart, the property route.
- Commits: `7a452036` (code), `cb0916e5` (docs), `4734f2bb` (task closed). Retry: none.

#### task-183-02-ore-hud-render (art) — result
- Files: `Art/Source/ore-hud/20261014-231429-ore-hud-1.png`, `-2.png` (untracked candidates), prompt `Art/Source/ore-hud/prompts/01-hud-render.txt`, run log `Art/Source/ore-hud/20261014-231429-ore-hud.json`. References: `Saved/Screenshots/WindowsEditor/Still_04.png` (grey-box arena, no HUD) and `Art/Source/stat-icons/damage-1.png`. 12 credits.
- Candidate 1: chunky faceted violet crystal cluster with a charcoal outline, in a compact dark plate right of the radar, white "27". Candidate 2: taller, sharper crystal overhanging its pill and overlapping the radar rim. Agent self-review: both pass A1, colour code, A12/A13; A14 (UI text in art) is a gap requested by the owner; it prefers 1 (reads better at HUD size).
- Git: the agent's commit hit a transient `.git/index.lock` (the Architect's concurrent commit); the Architect committed the prompt and run log: `f5698be8`. Retry: none.
- Sent to the owner; style-guide review running; waiting for the owner's choice.
- Style-guide review (`Art/Source/ore-hud/20261014-231429-ore-hud.review.json`, not committed): both fail. Shared: A14 (UI in reference art, owner-requested deviation), A6 (grey mannequin miner), colour-code (unexplained yellow and cyan spheres), readability (rocks without cracks). Image 2 also fails A1 (glossy, thin-shard crystal). Image 1's icon passes A1. Architect's note: the miner, the spheres and the uncracked rock come from the real grey-box screenshot used as reference (current in-game placeholders), not from the counter; the icon itself is what task-183-03 takes. Reported to the owner, who keeps or rejects each fail.

#### Owner answer on the render (verbatim)
> rendu 2, c'est très bien pour l'icone.
>
> Mais pour sa position sur l'écran, ça doit plus être comme l'or en haut à gauche, mais tu peux le faire plus grand que sur l'image

(The owner attached a screenshot of another top-down shooter as a placement reference: round minimap top-left; to its right, under a mission-title strip, a row of small resource counters, each an icon followed by a number, with no plate behind them.)

English translation: "Render 2, that's very good for the icon. But for its position on screen, it should be more like the gold at the top left, though you can make it bigger than in the image."

Decision: candidate 2 validated as the icon base, copied to `vault/assets/ore-hud-render.png`; its style-guide fails (A1 glossy crystal, A14 UI in art, and the scene fails) are kept by the owner's choice. Placement changed: an inline counter (icon then number, no plate) at the top, right of the minimap, larger than the reference; written into task-183-04. The icon brief now asks for a crystal that reads without a plate.

#### task-183-03-ore-hud-icon (art) — prompt sent
Reading list: the subtask node, then its `inputs` (agent-art, art-3d-lessons, art-style-guide, color-code, readability-rules, kie-image-workflow), `vault/assets/ore-hud-render.png`, one stat icon from `Art/Source/stat-icons/`.
```text
Subtask task-183-03-ore-hud-icon (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-183-ore-counter/task-183-03-ore-hud-icon.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- The owner chose candidate 2 of task-183-02 (2026-10-14, translated): "render 2, that's very good for the icon." It is now vault/assets/ore-hud-render.png. Draw the icon from the violet crystal cluster in its top-left counter (tall, sharp faceted shards, light violet glow): keep that crystal design. The style-guide critic flagged it A1 (glossy gradients, thin shards); the owner kept it on purpose: do not "fix" it toward flat chunky facets.
- Use: the icon sits directly on the game view at the top of the HUD, right of the round minimap, followed by the carried count, with no plate behind it (owner: like a gold counter at the top left, but larger). So it needs a silhouette and outline that read on dark and on light-grey backgrounds, at 48 to 64 px.
- Output: square (1:1), the crystal alone, centred with margin, on a plain flat background the bp agent can key out (or transparent if the model gives alpha). No text, no plate, no number.
- Use image-to-image with vault/assets/ore-hud-render.png (crop or describe the counter area) and one stat icon from Art/Source/stat-icons/ as references.
- Limits: at most 2 candidates; one kie_image.js run at a time, never in parallel; never write the API key anywhere; never put an image in vault/assets/.
- Self-review per image includes a 48 px downscale check (describe what still reads).
- Git: prefix every git command with `git -c "safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs"`. Do not commit candidate images; commit only the prompt file and run log by explicit path, message `task-183-03-ore-hud-icon: <summary>` ending with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. Never `git add -A`. If `.git/index.lock` exists, wait a few seconds and retry once; never delete it.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: candidate PNG paths, one line per candidate, self-review with the 48 px check, credits used, commit hash.
```

#### task-183-03-ore-hud-icon (art) — result
- Files: `Art/Source/ore-hud/20261014-233627-ore-hud-1.png`, `-2.png` (untracked candidates, 1:1, pure green #00FF00 key background, no alpha), prompt `Art/Source/ore-hud/prompts/03-hud-icon.txt`, run log `20261014-233627-ore-hud.json`. References: `vault/assets/ore-hud-render.png`, `Art/Source/stat-icons/mining-speed-2.png`. 12 credits. Commit `951a9bf4` (prompt and log). Retry: none.
- Candidate 1: one tall central shard, two leaning side shards, two low front shards, thick charcoal outline, cooler violet with lilac highlights; shards stay separate at 48 px. Candidate 2: same design, more saturated magenta-violet, heavier outline, shards merge slightly more at 48 px.
- Sent to the owner; style-guide review running; waiting for the owner's choice.
- Style-guide review (`Art/Source/ore-hud/20261014-233627-ore-hud.review.json`, not committed): both fail. Both: A1 gloss (owner-kept deviation, reminder) and no inner glow ("glowing violet crystals" in color-code). Candidate 2 also leans magenta, the colour reserved for the big XP block. Recommends candidate 1. Reported to the owner.

#### Owner answer on the icon (verbatim)
> Icône 2, copie-la dans vault/assets et lance le widget.

English translation: "Icon 2, copy it to vault/assets and launch the widget."

Decision: candidate 2 validated, copied to `vault/assets/ore-hud-icon.png`; its style-guide fails (A1 gloss, magenta lean, no glow) are kept by the owner's choice. Editor launched for task-183-04.

#### task-183-04-ore-counter-widget (bp) — prompt sent
Reading list: the subtask node and its `inputs` (agent-bp, bp-mcp-lessons, wbp-hud, wbp-minimap-locator, wbp-alert-objectives, sys-hud-events, color-code, readability-rules), plus `sys-objectives-ore-counter` and `sys-hud-bindings` (the C++ side), `vault/assets/ore-hud-icon.png`.
```text
Subtask task-183-04-ore-counter-widget (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-183-ore-counter/task-183-04-ore-counter-widget.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md. Also read vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md and vault/reference/tech/systems/ui/sys-hud/sys-hud-bindings.md (the C++ side built in task-183-01).

Notes:
- C++ API (task-183-01): `URRRunFlowSubsystem::OnOreChanged` (BlueprintAssignable delegate, params `int32 CarriedOre, int32 SecuredOre`), pure getters `GetCarriedOre()` and `GetSecuredOre()`. Bind it in the panel's `BindToRunFlow` hook (URRHUDPanelWidget) and read both getters once there; unbind on rebind and Destruct, like wbp-minimap-locator does with its delegates (Assign nodes). No tick, no polling.
- Icon: the owner chose vault/assets/ore-hud-icon.png (1:1, 1254 px class, crystal on a pure green #00FF00 key background, no alpha). Import it as a UI texture (TC_UserInterface2D / no mips) under /Game/RogueRigs/UI/Icons/ (or the folder the existing UI icons use); remove the green either in the texture through a masked UI material, or by keying it into an alpha channel before import (a Node script with an image library already in the repo is fine; no new dependency without saying so). The crystal edges must not keep a green fringe.
- Placement (owner 2026-10-14): top of the HUD, right of the round minimap (disc at x 20..220, y 20..220 on 1920 x 1080), an inline counter "icon then number" with no plate, like a gold counter at the top left of other games, but larger than that reference: suggested icon 56 x 56 and number about 32 bold white with a dark outline, at about x 236, y 30. It must not overlap the minimap, the health bar (y 232..260) or the alert bar (x 660..1260, y 48..84).
- Success screen in WBP_AlertObjectives: show the secured ore (e.g. icon + "Ore secured: N") using `GetSecuredOre()` when `OnRunEnded(true)` shows the success screen.
- Editor state: open (launched by the Orchestrator). Check the MCP server answers and no PIE runs first.
- Verification: compile with warnings as errors; read back the slot placement; in PIE run `rr.Cheat.GrantOre 10` (CVar; or the CDO property `GrantOre` on /Script/RogueRigs.Default__RRCheatSettings) and read back the counter text; then `bSkipToExtraction`, board the shuttle if you can (else report), and read the success screen text. No screenshots (your rules); say what is unverified.
- Git: prefix every git command with `git -c "safe.directory=C:/Users/soumo/Documents/Unreal Projects/RogueRigs"`. The owner has an unrelated uncommitted change to WBP_UpgradeChoice.uasset and others: never touch, save or commit them; commit only your files by explicit path. If `.git/index.lock` exists, wait and retry once; never delete it. Commit message `task-183-04-ore-counter-widget: <summary>` ending with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Docs: create the doc node `wbp-ore-counter` (template vault/templates/blueprint.md, parent blueprints-ui-index, next to wbp-minimap-locator), update `wbp-hud` (placement table, now 8 children) and `wbp-alert-objectives`; lint; commit.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: asset paths, how the green was removed, placement values read back, the PIE read-backs, tools that failed, commit hashes, what is unverified.
```

#### task-183-04-ore-counter-widget (bp) — result
- Files: `Art/UI/Icons/T_Icon_Ore.png` (green keyed to alpha, 256 px, 4 % margin; a scratch copy of `Tools/art/ui_icon_prepare.js` with green instead of magenta keys, not committed; `--check`: 0 green pixels left), `Content/RogueRigs/UI/Icons/T_Icon_Ore.uasset`, `Content/RogueRigs/UI/WBP_OreCounter.uasset` (new, parent `URRHUDPanelWidget`), `WBP_HUD.uasset` (8th child, z 6), `WBP_AlertObjectives.uasset` (end-screen row "Ore secured: N"); docs: new `wbp-ore-counter`, updated `wbp-hud`, `wbp-alert-objectives`, `blueprints-ui-index`.
- Layout: icon 56 × 56 at x 236, y 30; count Roboto Bold 32 white with black outline from x 300; no overlap with the minimap, health bar or alert bar (read back).
- Binding: `OnOreChanged` assigned in `BindToRunFlow`, one `ShowOre(GetCarriedOre)` at bind, unbound on rebind and Destruct; no tick, no timer.
- PIE read-backs (Slate text, no screenshots): counter "10" after `GrantOre 10`; after `bSkipToExtraction` and boarding: "EXTRACTION SUCCESSFUL" and "Ore secured: 10"; log "extraction secured 10 ore, secured 10". An earlier PIE run where the miner died emptied the 10 carried ore.
- Failures and retries inside the agent: several Blueprint DSL writes redone (positional argument shifted to `self`, IsValid ending the flow, custom event created without delegate parameters). No second agent run.
- Unverified: on-screen look (edge fringe, crispness, end-row layout), counter back to 0 after securing, rebind after restart. Doc leftover: `wbp-alert-objectives` purpose line still says z 20.
- Commits: `8a3a1f80` (assets and docs), `51a61480` (subtask closed), `17083710` (global result). Note: a commit of the Architect (183-03 choice) had silently failed during a parallel owner session's commit; redone after the check.

### 7. Rescan
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| ore-economy: carried ore counted, secured on extraction, lost on a miss | M1 | **built** | `Pickups/RRPickup.cpp:348` `RunFlow->AddCarriedOre(OreValue)`; `Objectives/RRRunFlowSubsystem.cpp:562` `SecureCarriedOre()`, `:566` `LoseCarriedOre("shuttle missed")`, `:765` `LoseCarriedOre("miner died")`; `Objectives/RROreBankSubsystem.h` `GetSecuredOre`, `AddSecuredOre` |
| extraction: ore secured on success, lost on a miss | M1 | **built** | same; success screen `Content/RogueRigs/UI/WBP_AlertObjectives.uasset` ("Ore secured: N") |
| objective-bonus-chest: 5 ore chunks reward | M2 | **built** | chest chunks are ore pickups, now counted by `ARRPickup::Collect` → `AddCarriedOre` |
| HUD ore display (owner addition) | M1 | **built** | `Content/RogueRigs/UI/WBP_OreCounter.uasset`, `Content/RogueRigs/UI/Icons/T_Icon_Ore.uasset`, `vault/reference/tech/blueprints/ui/wbp-ore-counter.md` |
| cheat | — | **built** | `Debug/RRCheats.cpp:163` `GrantOre`; CVar `rr.Cheat.GrantOre` |
| ore-economy: share of secured ore lost on a failed run | M2 | missing | still `open` |

Final agent commit (the "agent output" the owner's review is compared against): `17083710`.

### 8. Close
- task-183 closed per protocol (`status: done`, `next_step: none`, moved to tasks-done-index). state.md updated. Editor left open for the owner's test.
- Owner review edits go in a separate commit `owner review: ...`; this run stops here.

## Run 2 — 2026-10-16

### 1. Base
- Base commit: `d5b24055` (`task-195-03-dig-pickup-collect-test: done; next none`).
- Owner constraint for this run: none. Steps 1 to 3 ran first as `/architect-score` (read-only, answered in the chat); this run takes them over and records them here.
- Resume (orchestration-protocol step 0): no global task `in-progress`. One open task in tasks-to-do-index: `task-188-editor-lock` (status `todo`), not a GDD gap, not touched.
- Working tree clean at start.
- Unreal MCP server not reachable at start (editor closed): the editor is launched before the bp subtasks (runbook).

### 2. Scan
Method: same as run 1. Four read-only scan agents (run; player and weapons; enemies and terrain; art, audio and lore) over every `kind: spec` node (approved or provisional), evidence paths checked. Only the rows that changed since run 1, or that decide the ranking, are listed; the rest of the run 1 table holds.

| Spec / sub-feature | MS | Class | Evidence / gap |
|---|---|---|---|
| barrel: particle burst on destruction | M1 | missing | no Niagara reference in `Grid/RRArenaGrid.cpp`; only `ARRExplosionFX` (orange sphere, `BP_ExplosionFX`) plays |
| enemy-chaser: shield packs 2–4 among 8–16 from 1:00 | M1 | partial | `Enemies/RRWaveDirector.cpp:86-92`: wide pack matches, `ShieldChasers` pack is 1–2 among 4–5 |
| enemy-shooter: preferred distance 800 ± 1 cell (67) | M2 | partial | `Content/RogueRigs/Data/Tables/EnemyStats.csv` `PreferredDistanceTolerance` = 200 |
| objective-bonus-chest: gold icon on the alert bar | M2 | partial | `AlertIconChest` shows the letter "B" (`vault/reference/tech/blueprints/ui/wbp-alert-objectives.md`) |
| color-code: yellow logo plate on every robot | M1 | partial | heavy and shooter have one (`bp-heavy.md`, `bp-shooter.md`); kamikaze has none (`bp-kamikaze.md`) |
| visual-style: void with stars and planet | — | partial | `bp-arena-grid.md`: no sky; no star or planet asset |
| drone / crew-characters: barks shown through the drone | M3 | partial | `Content/RogueRigs/Data/Text/barks.json`, `Text/RRSubtitleSubsystem.cpp`; nothing displays barks |
| enemy-death: robot debris for heavy, kamikaze, shooter | M3 | missing | `DebrisPieces` set on chaser and shield chaser only; parts `open` |
| run-structure: 4 levels | M2 | missing | correction to run 1: blocked by the missing run controller (`RestartLevel` reloads the same map) and `open` values, not by the generator |
| ore-economy, extraction, xp-level-up, explosions, heal-ore, ore-blocks, turret, upgrades (stat cards), rarities | M1–M2 | built | unchanged since run 1; ore blocks, deposits and pickups refined by tasks 186–196 (polish, no gap closed) |

Colour conflicts found (not gaps): mining laser red `MI_GB_Laser` vs white-gold in `color-code`; critical number orange-gold `(1, 0.55, 0.08)` vs white-gold; big XP chip lilac core vs magenta trail. Doc drift: `sys-difficulty` names a missing `URRGE_EnemyDifficulty`; `bp-arena-grid` says CellSize 200 / `arena_m1`; `RREnemy` `DebrisPoolSize` default 40 vs 80 in the Blueprint.

### 3. Scoring
Factors as in run 1 (MS + P + U + C; dependencies not ready = 0; `open` = not buildable).

| # | Feature (spec) | MS | P | U | C | Total | Reason |
|---|---|---|---|---|---|---|---|
| 1 | Barrel particle burst (barrel) | 3 | 2 | 0 | 0 | **5** | approved M1 juice, fully specified, nothing blocks it; one main asset |
| 2 | Drone shows the barks (drone, crew-characters, lore L2) | 1 | 1 | 2 | 1 | 5 | data and subtitle subsystem exist; several subtasks |
| 3 | Shield-chaser pack makeup (enemy-chaser) | 3 | 1 | 0 | 1 | 5 | possible owner tuning: check before changing |
| 4 | C++ arena generator (arena-grid) | 2 | 2 | 1 | 0 | 5 | very large; only partly unblocks the 4-level run |
| 5 | Shooter tolerance (enemy-shooter) | 2 | 1 | 0 | 1 | 4 | one CSV value, owner's feel |
| 6 | Bonus chest gold icon (objective-bonus-chest) | 2 | 1 | 0 | 1 | 4 | one widget asset |
| 7 | Kamikaze logo plate (color-code) | 3 | 0 | 0 | 1 | 4 | passes no pillar |
| 8 | Void stars and planet (visual-style) | 1 | 1 | 0 | 1 | 3 | art assets needed |
| 9 | Stat sheet UI (player-stats) | 3 | 0 | 0 | 0 | 3 | passes no pillar |

Tie at 5 broken by size and readiness: the barrel is the smallest, fully specified, approved M1 item.
Excluded: threat, tier pool, special ore, ore-loss share, run-structure objective and ramp, magazine and turret-charge upgrades, effect cards (`open` values); 4-level run and carry-over (no run controller, `open`); damage-number toggle (no options menu); run log (`sys-logging` not designed); EMP (no stun); audio (no assets); miner look (Tripo asset not imported); robot parts (`open`); colour conflicts (owner decision).

### 4. Recommendation
- **Feature:** barrel particle burst on destruction ([barrel](../../vault/reference/gdd/terrain/barrel.md): "On destruction it plays a particle burst and explodes").
- **Why:** highest score, approved M1, P1 juice on the P3 arena-as-weapon object; the fuse, the telegraph and the explosion queue exist (`sys-grid-barrels`, `sys-explosions`).
- **`open` values:** none in the spec; particle values to be set provisional.

#### Owner answer (verbatim)
> 1. Va pour le baril. Il faudrait que pendant que la zone grandit, on voit qu'il est instable. Ensuite il faudrait une partie d'explosion et aussi le cercle d'explosion. ( On pourra re utiliser pour le kamikaze)

English translation: "1. Go for the barrel. While the zone grows, we should see that it is unstable. Then there should be an explosion part and also the explosion circle. (We can reuse it for the kamikaze.)"

Decision: **approved**: barrel. The owner widens it to three parts: (a) the barrel looks unstable during the 1.5 s fuse while the ground circle grows; (b) an explosion particle part; (c) an "explosion circle"; built so the kamikaze can reuse it. Before planning, questions put to the owner (each with a default): what "unstable" shows; whether the particle burst is added on top of the orange sphere; which circle (c) means (default: a shockwave ring on the ground growing to the explosion radius); kamikaze now or later (default: later, one table line); provisional values set by the agents.

#### Owner answer to the questions (verbatim)
> Ok

English translation: "Ok."

Decision: every default accepted.
1. Unstable during the 1.5 s fuse: the barrel shakes more and more (render only, like the dig shake but growing), and its body blinks orange-white faster and faster until the explosion. No spark or smoke jet.
2. Explosion part: a Niagara burst at the explosion (flash, orange fireball, sparks, a little smoke), scaled to the radius, on top of the orange sphere, which stays (it shows the real reach).
3. Explosion circle: reading (a), a shockwave ring on the ground, orange, growing from the centre to the explosion radius (700) at the explosion, lasting about as long as the sphere (0.6 s).
4. Kamikaze unchanged now: the burst and the ring are built in a separate barrel explosion FX, so the kamikaze takes them later by changing one `ExplosionStats` line.
5. Fine values (shake amplitude, blink rate, particle counts) set by the agents as provisional.

### 5. Plan
- GDD check (orchestration-protocol step 2): [barrel](../../vault/reference/gdd/terrain/barrel.md) (approved, M1), [explosions](../../vault/reference/gdd/terrain/explosions.md), [ground-telegraph](../../vault/reference/gdd/art-audio/ground-telegraph.md), [readability-rules](../../vault/reference/gdd/art-audio/readability-rules.md) (orange = explodes), [enemy-kamikaze](../../vault/reference/gdd/enemies/enemy-kamikaze.md). No conflict: the grey-box sphere of `explosions` stays; the ring is orange (danger colour), not red. No `open` value needed. Milestone M1. The owner's three parts are written into `barrel` as an owner rule (provisional, 2026-10-16).
- Facts that shape the split: barrels are grid cells drawn by a HISM, not actors (`barrel-asset`, `sys-grid-barrels`); the grid already shakes dig cells in render only (`SetCellShaking`, `ShakeAmplitude`) and writes per-instance custom data (marks, ore colour), so the blink is a custom data float read by the barrel body material `M_BarrelPaint` / `MI_Barrel_body`; every explosion spawns its row's `FXClass` (`ARRExplosionFX`, `BP_ExplosionFX`) with the visual radius, so a barrel-only FX class keeps the kamikaze unchanged.
- One-click test (architect step 3): new cheat `rr.Cheat.FuseBarrel 1` with a property route (MCP cannot set CVars): lights the fuse of the barrel nearest to the miner.
- Global task: `task-197-barrel-explosion-feedback`, milestone M1, one owner point, six subtasks because it spans two C++ systems and four editor assets (bp split rule: one main asset per subtask):

| # | Subtask | Agent | Depends on | Output |
|---|---|---|---|---|
| 1 | `task-197-01-barrel-fuse-instability` | cpp | — | growing render-only shake during the fuse, fuse progress in a per-instance custom data float, cheat `rr.Cheat.FuseBarrel`, docs |
| 2 | `task-197-02-explosion-fx-burst-shockwave` | cpp | 1 (same build) | `ARRExplosionFX`: optional Niagara burst (radius user parameter) and optional ground shockwave decal (radius, progress, opacity), both off when unset; docs |
| 3 | `task-197-03-barrel-fuse-blink-material` | bp | 1 | barrel body material blinks orange-white faster and faster from the custom data; `barrel-asset` doc |
| 4 | `task-197-04-barrel-explosion-niagara` | bp | 2 | `NS_BarrelExplosion` (flash, fireball, sparks, smoke) scaled by radius; doc |
| 5 | `task-197-05-shockwave-decal-material` | bp | 2 | orange ring decal material and instance; doc |
| 6 | `task-197-06-barrel-explosion-fx-blueprint` | bp | 3, 4, 5 | `BP_ExplosionFX_Barrel` (child of `BP_ExplosionFX`) with burst and ring; `ExplosionStats` Barrel row points to it; PIE read-back through the cheat; doc |

- Order: 1 then 2 (editor closed, one build at a time); editor launched; 3, 4, 5 in parallel (three different assets, the limit on one editor); then 6.
- Not planned (owner rules): no new automation test, no bot run, no screenshot session; kamikaze FX unchanged; spark or smoke jet during the fuse not built.
- Status: plan shown to the owner, waiting for confirmation (protocol step 4).

#### Owner confirmation (verbatim)
> Pas besoin de l'instruction test en un clic. Mais tu peux au moins vérifier visuellement si les effets se lance bien in game.
> Tu peux lancer.

English translation: "No need for the one-click test instruction. But you can at least check visually that the effects start properly in game. You can launch."

Decision: plan confirmed and launched without asking again; task-197 set `in-progress`. Changes from the shown plan:
- No cheat: `rr.Cheat.FuseBarrel` removed from subtask 1; no new cheat or debug command is committed. PIE read-backs in subtasks 3 and 6 trigger a fuse through the existing BlueprintCallable `ARRArenaGrid::DestroyCell` (or `ApplyShotDamage`) on a barrel cell.
- Added subtask 7 (owner request): `task-197-07-barrel-fx-visual-check` (cpp agent, editor closed, `-game` run with `rr.Debug.Shots`): screenshots of one barrel fuse and explosion with the existing debug commands; the Architect looks at the frames and reports what is seen. If no existing route can light a barrel in a `-game` run, the agent stops and reports instead of adding a command.

### 6. Build

#### task-197-01-barrel-fuse-instability (cpp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, ground-telegraph, sys-grid, sys-grid-barrels, sys-grid-api, sys-automation-tests, barrel-asset, bp-arena-grid, agent-cpp, build-toolchain), resolved paths in the node.
```text
Subtask task-197-01-barrel-fuse-instability (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-01-barrel-fuse-instability.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Owner request (2026-10-16, translated): "While the zone grows, we should see that the barrel is unstable." This subtask is the C++ side only: a render-only shake that grows with fuse progress, and the fuse progress written into a per-instance custom data float of the barrel HISM instance for the material blink built in task-197-03 (bp). Pick the custom data index so it does not collide with the marked float or the ore colour floats, expose it with a getter, and write it in sys-grid-barrels.
- Keep the game rules unchanged: fuse duration, damage immunity during the fuse, explosion timing and the ground telegraph stay as they are. Only render.
- Values (amplitudes, any growth curve) are provisional: `EditAnywhere`, written in the doc as "provisional, owner 2026-10-16".
- Editor state: closed (checked: no UnrealEditor process). Build from the command line per build-toolchain. If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Scope: exactly the subtask goal. No cheat, no debug command (owner 2026-10-16), no new automation test; only update expectations of existing tests your change breaks (for example a test that expects the shake to stop at fuse start).
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`. Commit message `task-197-01-barrel-fuse-instability: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: files changed, the custom data index and its getter, the shake properties with their values, build result, test results (names, pass/fail, any expectation changed and why), docs updated, commit hash(es), what is unverified.
```

#### task-197-01-barrel-fuse-instability (cpp) — result
- Files: `Source/RogueRigs/Grid/RRArenaGrid.h`, `RRArenaGrid.cpp`; docs `sys-grid-barrels`, `sys-grid-api`.
- Fuse progress (0..1) written each grid tick into per-instance custom data float `BarrelFuseCustomDataIndex` = 4 (getter `GetBarrelFuseCustomDataIndex`, BlueprintPure; 0 = marked, 1–3 = ore colour); reset at fuse start and end, rebuild starts at 0. Helper `WriteCellCustomData`.
- Shake (render only, provisional): horizontal 2 → 12 cm, vertical 1 → 5 cm, `lerp(start, end, progress^2)`, `ShakeFrequency` 8 Hz reused; the drone cannot stop a fusing barrel's shake. Game rules unchanged.
- Build exit 0. Tests: 29/30 pass (Barrel, BarrelFuse, Dig, Explosions, Ore). `RogueRigs.Ore.HealVein` fails, also on the code before the change (2 of 4 runs pass before, 1 of 4 after): flaky, unrelated, reported to the owner, not fixed (no unrequested work).
- Commits `297aeafe` (code), `e5af6cb0` (docs). No retry.
- Unverified: the look of the shake; no material reads index 4 yet (task-197-03).

#### task-197-02-explosion-fx-burst-shockwave (cpp) — prompt sent
Reading list: the subtask node, then its `inputs` (explosions, barrel, enemy-kamikaze, sys-explosions, bp-explosion-fx, sys-ground-telegraph, ground-telegraph, readability-rules, sys-automation-tests, agent-cpp, build-toolchain), resolved paths in the node.
```text
Subtask task-197-02-explosion-fx-burst-shockwave (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-02-explosion-fx-burst-shockwave.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Owner request (2026-10-16, translated): "Then there should be an explosion part and also the explosion circle (we can reuse it for the kamikaze)." Defaults accepted: a Niagara burst on top of the orange sphere, and an orange shockwave ring on the ground growing from the centre to the explosion radius, about as long as the sphere. This subtask is the C++ side only, in `ARRExplosionFX`: the bp agent then builds `NS_BarrelExplosion` (task-197-04), the ring decal material (task-197-05) and `BP_ExplosionFX_Barrel` (task-197-06) that sets them. So: generic, optional, unset = exactly today's behaviour (`BP_ExplosionFX`, used by the kamikaze, must not change).
- Names you choose (Niagara property, its float user parameter for the visual radius, decal material property or component, material parameter names for radius, progress and opacity) are the contract for the bp subtasks: list them all in sys-explosions. Prefer the ground telegraph's existing parameter naming (sys-ground-telegraph) where it fits.
- The ring is a projected decal like the ground telegraph (floor and rock tops). Its decal box must cover the visual radius; progress grows over the sphere growth (`Duration`), opacity follows the sphere fade. The actor must live long enough for the Niagara burst to finish (about 1.5 s) when a burst is set, without changing the lifetime when it is not.
- Values are provisional: written in the doc as "provisional, owner 2026-10-16".
- Editor state: closed (checked: no UnrealEditor process). Build from the command line per build-toolchain. If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Scope: exactly the subtask goal. No cheat, no debug command (owner 2026-10-16), no new automation test; only update expectations of existing tests your change breaks. Known unrelated flaky test: `RogueRigs.Ore.HealVein` (fails before this task too): report it, do not fix it.
- The previous subtask (task-197-01) changed `Source/RogueRigs/Grid/RRArenaGrid.*`; do not touch the grid.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`. Commit message `task-197-02-explosion-fx-burst-shockwave: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: files changed, the exact property names, user parameter name, material parameter names and decal settings (size, sort order, rotation), the lifetime rule, build result, test results, docs updated, commit hash(es), what is unverified.
```

#### task-197-02-explosion-fx-burst-shockwave (cpp) — result
- Files: `Source/RogueRigs/Explosions/RRExplosionFX.h`, `RRExplosionFX.cpp`; docs `sys-explosions`, `bp-explosion-fx`.
- Contract for the bp subtasks: `BurstSystem` (one-shot Niagara, played once at BeginPlay at the centre, float user parameter `User.Radius` = visual radius via `BurstRadiusParameterName`, `BurstSeconds` 1.5); `ShockwaveMaterial` (deferred decal, pitch −90, box half size (256, Radius, Radius), sort order 1; scalars `Radius`, `Progress` 0→1 over `Duration` 0.25 s, `Opacity` following the sphere fade). Lifetime: unchanged (0.6 s) without a burst, 1.5 s with one. Unset = old behaviour (kamikaze unchanged).
- Build exit 0. Full suite headless: 260 pass, 2 fail (`Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`), both failing on the previous `RRExplosionFX` too: pre-existing, reported, not fixed. `Ore.HealVein` passed this time (flaky).
- Commits `09946e84` (code), `d1ce7a58` (docs). No retry.
- Unverified: nothing uses the properties yet; Niagara never runs headless.

Editor launched by the Architect for the bp subtasks.

#### task-197-03-barrel-fuse-blink-material (bp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, barrel-asset, bp-arena-grid, sys-grid-barrels, color-code, readability-rules, agent-bp, bp-mcp-lessons, unreal-mcp-quirks), resolved paths in the node.
```text
Subtask task-197-03-barrel-fuse-blink-material (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-03-barrel-fuse-blink-material.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Owner request (2026-10-16, translated): "While the zone grows, we should see that the barrel is unstable." Default accepted: its body blinks orange-white faster and faster until the explosion (the C++ shake is already done).
- Contract from task-197-01 (see sys-grid-barrels): the grid writes the fuse progress 0..1 into per-instance custom data float index 4 (`ARRArenaGrid::GetBarrelFuseCustomDataIndex`, `BarrelFuseCustomDataIndex`) of the barrel HISM instance; 0 when not fusing. The grid sizes the barrel HISM custom data to index + 1 floats.
- The barrel body is `/Game/RogueRigs/Art/Barrel/M_BarrelPaint` through `MI_Barrel_body` (slot 0, overridden by the grid's `BarrelMaterial`); other `MI_Barrel_*` instances share the parent: they must keep their look (gate the effect with a static switch or a parameter only the body instance enables, or with the custom data value which is 0 on non-instanced meshes). Read the value with a PerInstanceCustomData node (index 4, default 0). Blink: emissive orange-white pulse whose frequency rises from slow to fast with progress (for example about 2 Hz at 0 to about 12 Hz at 1, provisional), off at progress 0 so a resting barrel looks exactly as before.
- PIE read-back, if the editor allows it: in PIE call the BlueprintCallable `ARRArenaGrid::DestroyCell` on a barrel cell (source any) and read the custom data back rising 0 to 1; stop PIE before saving. If PIE probing fails, report it as unverified rather than looping.
- Values are provisional: write them in the doc as "provisional, owner 2026-10-16". Colours follow color-code and readability-rules (orange = explodes; red is reserved for enemy fire).
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp). Two other bp agents work in parallel on the same editor on other assets (task-197-03 barrel body material, task-197-04 NS_BarrelExplosion, task-197-05 shockwave decal material): touch only your own asset(s); never save all; save by explicit asset path. Check `IsPIERunning` before saving; if you need PIE, coordinate by keeping it short and stopping it before you save.
- Never call a viewport or editor capture tool (bp-mcp-lessons).
- Scope: exactly the subtask goal. No cheat, no debug command, no new test (owner 2026-10-16).
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; `git add` new files by name first. Commit message `task-197-03-barrel-fuse-blink-material: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. Run `node vault/scripts/lint.js --write` after editing the vault and fix what it reports.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets created or changed (paths), parameters and values read back, MCP tools that failed, docs updated, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
```

#### task-197-04-barrel-explosion-niagara (bp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, explosions, sys-explosions, bp-explosion-fx, bp-robot-death-puff, blueprints-fx-index, color-code, readability-rules, agent-bp, bp-mcp-lessons, unreal-mcp-quirks), resolved paths in the node.
```text
Subtask task-197-04-barrel-explosion-niagara (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-04-barrel-explosion-niagara.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Owner request (2026-10-16, translated): "Then there should be an explosion part (we can reuse it for the kamikaze)." Default accepted: a Niagara burst at the explosion (short bright flash, orange fireball, sparks thrown outward, a little dark smoke), scaled to the radius, playing on top of the existing orange sphere.
- Contract from task-197-02 (see sys-explosions): `ARRExplosionFX::BurstSystem` plays the system once at the explosion centre at BeginPlay, sets the float user parameter `User.Radius` (a float named `Radius` in the User namespace) to the visual radius (735 uu for the barrel, 327.6 uu for the kamikaze if reused later); the actor keeps it alive for `BurstSeconds` = 1.5 s. Declare `User.Radius` with a sensible default (735) and size every emitter from it so the effect fills about the visual radius for both sizes.
- One-shot, finished within about 1.5 s, no collision, the cheapest of CPU or GPU sim; keep particle counts modest (barrel chains can trigger several in a row). Look at NS_RobotDeathPuff (bp-robot-death-puff) for how an existing burst was built through MCP. Save as `/Game/RogueRigs/FX/NS_BarrelExplosion`; new doc node in vault/reference/tech/blueprints/fx/ (template vault/templates/blueprint.md adapted as the other fx docs do) and blueprints-fx-index updated.
- Values are provisional: write them in the doc as "provisional, owner 2026-10-16". Colours follow color-code and readability-rules (orange = explodes; red is reserved for enemy fire).
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp). Two other bp agents work in parallel on the same editor on other assets (task-197-03 barrel body material, task-197-04 NS_BarrelExplosion, task-197-05 shockwave decal material): touch only your own asset(s); never save all; save by explicit asset path. Check `IsPIERunning` before saving; if you need PIE, coordinate by keeping it short and stopping it before you save.
- Never call a viewport or editor capture tool (bp-mcp-lessons).
- Scope: exactly the subtask goal. No cheat, no debug command, no new test (owner 2026-10-16).
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; `git add` new files by name first. Commit message `task-197-04-barrel-explosion-niagara: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. Run `node vault/scripts/lint.js --write` after editing the vault and fix what it reports.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets created or changed (paths), parameters and values read back, MCP tools that failed, docs updated, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
```

#### task-197-05-shockwave-decal-material (bp) — prompt sent
Reading list: the subtask node, then its `inputs` (ground-telegraph, sys-ground-telegraph, sys-explosions, explosions, color-code, readability-rules, blueprints-fx-index, agent-bp, bp-mcp-lessons, unreal-mcp-quirks), resolved paths in the node.
```text
Subtask task-197-05-shockwave-decal-material (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-05-shockwave-decal-material.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Owner request (2026-10-16, translated): "and also the explosion circle (we can reuse it for the kamikaze)." Default accepted: an orange shockwave ring on the ground growing from the centre to the explosion radius at the explosion, lasting about as long as the sphere.
- Contract from task-197-02 (see sys-explosions): `ARRExplosionFX::ShockwaveMaterial` is used on a decal component projected straight down (pitch -90), box half size (256, Radius, Radius), sort order 1; the C++ sets scalar parameters `Radius` (visual radius, uu, set once), `Progress` (0 to 1 over 0.25 s, then 1) and `Opacity` (1 during growth and hold, then 1 to 0 over the fade). Same names as `/Game/RogueRigs/FX/M_GroundTelegraph`: look at it (sys-ground-telegraph) and reuse its approach for a sharp ring of constant world width at radius x progress.
- Build `/Game/RogueRigs/FX/M_ExplosionShockwave` (deferred decal, translucent, unlit emissive) and an orange instance `MI_ExplosionShockwave` (provisional ring width in uu, orange per color-code); invisible at progress 0, no disc fill. New doc node in vault/reference/tech/blueprints/fx/ and blueprints-fx-index updated.
- Values are provisional: write them in the doc as "provisional, owner 2026-10-16". Colours follow color-code and readability-rules (orange = explodes; red is reserved for enemy fire).
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp). Two other bp agents work in parallel on the same editor on other assets (task-197-03 barrel body material, task-197-04 NS_BarrelExplosion, task-197-05 shockwave decal material): touch only your own asset(s); never save all; save by explicit asset path. Check `IsPIERunning` before saving; if you need PIE, coordinate by keeping it short and stopping it before you save.
- Never call a viewport or editor capture tool (bp-mcp-lessons).
- Scope: exactly the subtask goal. No cheat, no debug command, no new test (owner 2026-10-16).
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; `git add` new files by name first. Commit message `task-197-05-shockwave-decal-material: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. Run `node vault/scripts/lint.js --write` after editing the vault and fix what it reports.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets created or changed (paths), parameters and values read back, MCP tools that failed, docs updated, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
```

Subtasks 03, 04 and 05 delegated in parallel (three different assets on one editor, the bp limit).

#### task-197-05-shockwave-decal-material (bp) — result
- Assets: `/Game/RogueRigs/FX/M_ExplosionShockwave` (copy of `M_GroundTelegraph`, ring rewritten; Deferred Decal, Translucent, Unlit), `/Game/RogueRigs/FX/MI_ExplosionShockwave` (orange 1, 0.45, 0.05). Parameters `Radius` (700), `Progress` (0), `Opacity` (1); one solid ring 24 uu wide (provisional) just inside radius × progress, invisible at 0; dashes and fill return 0.
- Doc: new `bp-explosion-shockwave`, `blueprints-fx-index` updated. MCP through JSON-RPC (tools not exposed to the agent); one unreadable property, no failure.
- Commit `8433d794`. No retry.
- Unverified: look on screen; the C++ decal driving the parameters in game (task-197-06, task-197-07).

#### task-197-03-barrel-fuse-blink-material (bp) — result
- Asset: `/Game/RogueRigs/Art/Barrel/M_BarrelPaint` (master): PerInstanceCustomData index 4 (default 0) → emissive lerp orange (1, 0.4, 0.05) to white-hot (1, 0.95, 0.85) × blink × 3; blink rate 2 → 12 Hz integrated into a phase over `FuseDuration` 1.5 s (must follow the grid's `BarrelFuseSeconds`); group `FuseBlink`, all provisional. Exactly 0 at progress 0; non-instanced meshes read 0, so other `MI_Barrel_*` keep their look. `bUsedWithInstancedStaticMeshes` set.
- Docs: `barrel-asset` (fuse blink section), `bp-arena-grid` (note).
- MCP failures inside the agent: wrong property names on PerInstanceCustomData, unnamed inputs ("None"), a git index.lock from a parallel agent; all retried in the same run.
- Commits `4f491333` (asset), `8fbdc832` (docs). No second run.
- Unverified: the PIE read-back of the custom data (the agent found no MCP route to call `DestroyCell`); the look. Both are covered by the `-game` visual check (task-197-07). Accepted on that basis.

#### task-197-04-barrel-explosion-niagara (bp) — result
- Assets: `/Game/RogueRigs/FX/NS_BarrelExplosion` (`User.Radius` float, default 735) and `/Game/RogueRigs/FX/M_BarrelExplosionSprite` (added by the agent: the engine default sprite material is additive and square, so dark smoke would be invisible; translucent, unlit, soft round blob). Accepted: a support asset of the same system, not a new feature.
- Emitters (CPU sprites, no shadow, sizes and speeds × `User.Radius`, provisional): Flash 1, Fireball 10, Sparks 28 (gravity), Smoke 6 at 0.1 s; 45 particles; last particle ends about 1.4 s (inside `BurstSeconds` 1.5); no red.
- Doc: new `bp-barrel-explosion`, `blueprints-fx-index` updated. MCP through JSON-RPC; no tool failure.
- Commits `50fdc0d8` (assets), `d7550dce` (docs). No retry.
- Unverified: look and density at 735 uu, cost in chains, component release (task-197-07 covers the look).

#### task-197-06-barrel-explosion-fx-blueprint (bp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, explosions, sys-explosions, bp-explosion-fx, blueprints-fx-index, agent-bp, bp-mcp-lessons, unreal-mcp-quirks), plus `bp-barrel-explosion` and `bp-explosion-shockwave`.
```text
Subtask task-197-06-barrel-explosion-fx-blueprint (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-06-barrel-explosion-fx-blueprint.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md. Also read the doc nodes written by the previous subtasks: vault/reference/tech/blueprints/fx/bp-barrel-explosion.md and vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md.

Notes:
- Owner request (2026-10-16, translated): an explosion particle part and an explosion circle for the barrel, reusable later by the kamikaze. This subtask assembles them: new `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel`, child of `BP_ExplosionFX` (sphere mesh and MI_GB_Explosion kept), with class defaults `BurstSystem` = `/Game/RogueRigs/FX/NS_BarrelExplosion` and `ShockwaveMaterial` = `/Game/RogueRigs/FX/MI_ExplosionShockwave` (property names per sys-explosions, task-197-02). No graph logic.
- Data: in `Content/RogueRigs/Data/Tables/ExplosionStats.csv` set row `Barrel` `FXClass` to `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel.BP_ExplosionFX_Barrel_C`; row `Kamikaze` unchanged. Check whether the CSV is imported into a DataTable asset that must be reimported, or read directly at runtime (sys-explosions: `FRRBaseStatsLoader::FindRow`), and do what the pipeline needs. Check no automation test hard-codes the Barrel FXClass path; if one does, report it (do not edit C++ tests yourself).
- Read back: the Blueprint's class defaults (both properties), the CSV rows. PIE: the bp agent of task-197-03 found no MCP route to call `ARRArenaGrid::DestroyCell`; try once to read, in PIE, that the explosion subsystem resolves the class of the Barrel row to the new Blueprint (for example `GetExplosionStats` if callable); if no route exists, say so and stop there: the in-game check is task-197-07 (a `-game` run with screenshots by the cpp agent). Do not loop on PIE.
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp). No other agent is working now. Check `IsPIERunning` before saving; save by explicit asset path; never save all.
- Never call a viewport or editor capture tool (bp-mcp-lessons).
- Scope: exactly the subtask goal. No cheat, no debug command, no new test (owner 2026-10-16).
- Docs: new `bp-explosion-fx-barrel` node in vault/reference/tech/blueprints/fx/ (template vault/templates/blueprint.md); update `bp-explosion-fx` (child exists), `sys-explosions` (data line: Barrel FXClass), `blueprints-fx-index`. Run `node vault/scripts/lint.js --write` and fix what it reports.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; `git add` new files by name first. Commit message `task-197-06-barrel-explosion-fx-blueprint: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets created or changed (paths), values read back, CSV change and how it is loaded, any test referencing the path, PIE result or why none, docs updated, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
```

#### task-197-06-barrel-explosion-fx-blueprint (bp) — result
- Asset: `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel` (parent `BP_ExplosionFX_C`), class defaults read back: `BurstSystem` = `NS_BarrelExplosion`, `ShockwaveMaterial` = `MI_ExplosionShockwave`; no graph logic.
- Data: `ExplosionStats.csv` Barrel `FXClass` → `BP_ExplosionFX_Barrel_C`; Kamikaze unchanged; CSV read at runtime (no DataTable reimport).
- Test broken by the requested change: `Tests/RRIntegrationTests.cpp` (`ExplosionFXClassPath`, line ~886; `Record.Class == FXClass`, line ~1093). Not edited by bp (no C++); handed to the cpp agent as step 1 of task-197-07 (owner rule: update only broken expectations).
- PIE: no MCP route to call a UFunction or read the resolved class; not started.
- Docs: new `bp-explosion-fx-barrel`; `bp-explosion-fx`, `sys-explosions`, `blueprints-fx-index` updated. Commit `7003f613`. No retry.
- Editor closed by the Architect for task-197-07 (git status clean of tracked assets before closing).

#### task-197-07-barrel-fx-visual-check (cpp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, sys-grid-barrels, sys-explosions, sys-cheats, agent-cpp, build-toolchain), plus `bp-explosion-fx-barrel`, `bp-barrel-explosion`, `bp-explosion-shockwave`, `barrel-asset`.
```text
Subtask task-197-07-barrel-fx-visual-check (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-197-barrel-explosion-feedback/task-197-07-barrel-fx-visual-check.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md. Also read the docs of what you are checking: vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md, bp-barrel-explosion.md, bp-explosion-shockwave.md, and the fuse blink section of vault/reference/tech/pipelines/barrel-asset.md.

Two steps, in this order:

1. Test expectation broken by task-197-06 (owner rule: update only the expectations a requested change breaks). `Source/RogueRigs/Tests/RRIntegrationTests.cpp` hard-codes `ExplosionFXClassPath` = `BP_ExplosionFX_C` (line ~886) and the barrel chain test asserts `Record.Class == FXClass` (line ~1093). The Barrel row now spawns `BP_ExplosionFX_Barrel_C` (child of `BP_ExplosionFX`, with a Niagara burst, so its actor lifetime is 1.5 s instead of 0.6 s: check lifetime expectations too). Make the barrel test expect the barrel class (and its lifetime) while kamikaze expectations keep `BP_ExplosionFX_C`. Build, run the Explosions, BarrelFuse and Integration explosion tests headless, commit (`task-197-07-barrel-fx-visual-check: <summary>`). Pre-existing unrelated failures: `Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`, flaky `Ore.HealVein`: report, do not fix.

2. Visual check (owner 2026-10-16, translated: "check visually that the effects start properly in game"). One `-game` run (not `-nullrhi`, Niagara must render) with `rr.Debug.Shots` (and `rr.Debug.Delayed` to chain steps): light one barrel fuse in view of the camera using existing routes only (for example `rr.Debug.SpawnNear` of a kamikaze next to a barrel at a place where the miner stands near a barrel, or `rr.Debug.Objective`, or the engine `ke` command), and capture frames during the fuse (shake, orange-white blink, growing danger circle) and during the explosion (Niagara burst, orange shockwave ring on the ground, orange sphere): for example a frame every 0.1–0.2 s from fuse start to 0.6 s after the explosion. No new cheat or debug command committed (owner 2026-10-16); a temporary uncommitted helper is also not allowed. If no existing route can put a fusing barrel in view, stop and report the options instead. Look at the frames yourself (Read the PNGs, downscale first if they are large) and describe what each shows.

Notes:
- Editor state: closed (the Orchestrator closed it). If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Git: commit only your files, by explicit path, never `git add -A` or `-- vault`. Do not commit the screenshots. Messages end with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: test change and results, commit hash; the exact command line of the -game run; the frame paths with one line each of what is visible (shake, blink colour, danger circle, burst, ring, sphere) and what is not; the log lines of the fuse and the explosion (FX class spawned); anything that looks wrong (too big, too dark, red, invisible ring, ...).
```

#### task-197-07-barrel-fx-visual-check (cpp) — result
- Step 1 (expectation broken by task-197-06): `Tests/RRIntegrationTests.cpp` barrel chain test now expects `BP_ExplosionFX_Barrel_C`, sphere-and-ring lifetime 0.6 s and actor lifetime 1.5 s; the unused `ExplosionFXClassPath` removed (no kamikaze test checked the FX class); doc `sys-tests-run-integration` updated. Build exit 0; 15/15 pass (Explosions ×8, BarrelFuse ×3, 4 integration explosion tests). Log: `FX 0 (BP_ExplosionFX_Barrel_C): seen at 1.6333 s, radius 735, destroyed 1.5000 s later (lifetime 1.50, visual 0.60)`. Commits `62c44756`, `14e493f3`.
- Step 2: one `-game` run, `L_ArenaM1`, 1280 × 720; existing routes only: `ke BP_Miner_C K2_AddActorWorldOffset` to stand next to the barrel at cell (145, 124), `ke BP_ArenaGrid_C ApplyShotDamage (X=145,Y=124) 100`, `rr.Debug.Shots 9.8 18 0.15 barrelfx`. Frames in `Saved/Screenshots/WindowsEditor/barrelfx_*.png` (not committed).
- What the Architect saw in the frames (01, 05, 11, 12, 13 opened):
  - fuse: the barrel alternates orange and white-cream (blink seen); the dashed danger circle and its growing fill are drawn; the shake cannot be judged from stills.
  - explosion: frame 12 shows the large orange-yellow sphere with a faint thin arc inside it (probably the ring) and small bright particles; frame 13 shows the fireball glow and orange sparks at the centre, sphere gone; smoke barely visible.
  - wrong: (a) the first frame of each decal (fuse start 01, explosion 11) shows a large pale green concrete square over the area; most likely the engine's default decal material while the decal shaders compile on first use in this run (the danger circle renders normally from the next frame), not verified by a second run; (b) the shockwave ring is hardly readable: 24 uu wide and it grows in 0.25 s under the translucent sphere, so the sphere hides it; (c) the burst is small next to the 735 uu sphere.
- Decision: subtask done (its `done_when` is met: frames cover the fuse and the explosion and were looked at). Points (a) to (c) are reported to the owner with proposals; nothing is changed without his answer (owner rule: no unrequested work).

### 7. Rescan
| Spec / sub-feature | MS | Class | Evidence |
|---|---|---|---|
| barrel: particle burst on destruction | M1 | **built** | `Content/RogueRigs/Data/Tables/ExplosionStats.csv` Barrel `FXClass` = `BP_ExplosionFX_Barrel_C`; `Content/RogueRigs/Blueprints/FX/BP_ExplosionFX_Barrel.uasset`; `Content/RogueRigs/FX/NS_BarrelExplosion.uasset`; `Source/RogueRigs/Explosions/RRExplosionFX.h` `BurstSystem`; test log line above; seen in frames 12–13 |
| barrel (owner rule 2026-10-16): unstable during the fuse | M1 | **built** (blink seen, shake not judged) | `Source/RogueRigs/Grid/RRArenaGrid.h` `BarrelFuseShake*`, `BarrelFuseCustomDataIndex` 4; `Content/RogueRigs/Art/Barrel/M_BarrelPaint.uasset` |
| barrel (owner rule 2026-10-16): shockwave ring on the ground | M1 | **partial** | `RRExplosionFX.h` `ShockwaveMaterial`; `Content/RogueRigs/FX/M_ExplosionShockwave.uasset`, `MI_ExplosionShockwave.uasset`; drawn but hardly visible under the sphere |
| kamikaze explosion | M1 | unchanged | `ExplosionStats.csv` Kamikaze `FXClass` = `BP_ExplosionFX_C` |

Final agent commit (the "agent output" the owner's review is compared against): `14e493f3`.

### 8. Close
- task-197 closed per protocol (`status: done`, `next_step: none`). state.md updated. Editor closed (the run left it closed for the C++ check).
- Reported to the owner: green decal square on first use (probable shader compile), ring hidden by the sphere, small burst; flaky or failing tests found on the way (`Ore.HealVein`, `Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`), all pre-existing.
- Owner review edits go in a separate commit `owner review: ...`.

#### Owner answer to the follow-up proposals (verbatim)
> Ok

English translation: "Ok."

Proposals accepted (defaults): (1) ring wider, about 60 uu, and lasting after the sphere: growth from 0.25 s to about 0.6 s; (2) bigger burst: wider fireball, about twice the sparks, more visible smoke; (3) a second `-game` run to confirm the green decal square is the shader compile on first use, nothing fixed if it is.

Decision: three owner points = three global tasks (split rule), confirmed by this "Ok" (the proposals were the plan); they run without asking again:

| Task | Subtasks | Agent | Output |
|---|---|---|---|
| `task-198-shockwave-ring-readable` | 01 ring timing in `ARRExplosionFX` (grow about 0.6 s, then a short fade, independent of the sphere, still inside the 1.5 s actor life) | cpp | C++, docs, test expectations only if broken |
| | 02 ring width 60 uu as a material scalar parameter `RingWidth` | bp | `M_ExplosionShockwave`, doc |
| `task-199-barrel-burst-bigger` | 01 `NS_BarrelExplosion`: wider fireball, about 2× sparks, more visible smoke, still within 1.5 s | bp | Niagara asset, doc |
| `task-200-decal-green-square-check` | 01 rerun of the task-197-07 `-game` command (shaders now cached), frames at fuse start and explosion; report whether the green square returns; no fix | cpp | report only |

Order: 198-01 (editor closed); editor launched; 198-02 and 199-01 in parallel (different assets); editor closed; 200-01 last, so its frames also show the new ring and burst. Values provisional (owner 2026-10-16).

#### task-198-01-shockwave-ring-timing (cpp) — prompt sent
Reading list: the subtask node, then its `inputs` (barrel, explosions, sys-explosions, bp-explosion-fx, bp-explosion-fx-barrel, bp-explosion-shockwave, sys-automation-tests, agent-cpp, build-toolchain).
```text
Subtask task-198-01-shockwave-ring-timing (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-198-shockwave-ring-readable/task-198-01-shockwave-ring-timing.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Why: in the task-197-07 `-game` frames the orange shockwave ring was hardly visible: it grows in 0.25 s under the translucent sphere, which hides it, and disappears with the sphere. Owner (2026-10-16) accepted: the ring lasts after the sphere, growth from 0.25 s to about 0.6 s. A bp subtask (task-198-02) widens the ring in the material at the same time; do not touch assets.
- What: give the shockwave its own timeline in `ARRExplosionFX` (Source/RogueRigs/Explosions/RRExplosionFX.*): `ShockwaveGrowSeconds` (0.6) for `Progress` 0 to 1, then `ShockwaveFadeSeconds` (0.2) for `Opacity` 1 to 0; both `EditDefaultsOnly`, provisional, owner 2026-10-16. The sphere keeps its own timeline (0.25 grow, 0.1 hold, 0.25 fade). The ring stays drawn after the sphere is hidden. Lifetime: with a burst it is already 1.5 s (enough); without a burst and with a shockwave, the lifetime is the longer of the sphere and the ring; with neither, unchanged (BP_ExplosionFX, kamikaze).
- Tests: run the Explosions, BarrelFuse and integration explosion tests (`ShotBarrelChainOpensWall` checks the barrel FX lifetimes); update only expectations this change breaks; no new test. Known unrelated failures: `Ore.HealVein` (flaky), `Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`: do not fix.
- Docs: `sys-explosions` (timing names and values), `bp-explosion-shockwave` (timing line only; the bp agent edits its width line in parallel: change only your line, commit by path).
- Editor state: closed (checked: no UnrealEditor process). Build from the command line per build-toolchain. If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Scope: exactly this. No cheat, no debug command.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`. Commit message `task-198-01-shockwave-ring-timing: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: files changed, property names and values, lifetime rule, build result, test results (and any expectation changed), docs, commit hash(es), what is unverified.
```

#### task-198-01-shockwave-ring-timing (cpp) — result
- `ARRExplosionFX`: `ShockwaveGrowSeconds` 0.6 (`Progress` 0→1), `ShockwaveFadeSeconds` 0.2 (`Opacity` 1→0), `GetShockwaveLifetime` 0.8 s; sphere timeline unchanged (hidden at 0.6 s), ring hidden at 0.8 s; lifetime = max(sphere, ring if set, burst if set): barrel 1.5 s, kamikaze unchanged. Provisional, owner 2026-10-16.
- Build succeeded. Explosions ×8, BarrelFuse ×3, `ShotBarrelChainOpensWall` pass; `Integration.JuiceHealRing` fails (pre-existing). Test file: comments only.
- Docs `sys-explosions`, `bp-explosion-shockwave` (timing line). Commits `c532ff13`, `d09f80d0`. No retry.
- Editor launched by the Architect for 198-02 and 199-01.

#### task-198-02-shockwave-ring-width (bp) — prompt sent
Reading list: the subtask node and its `inputs`.
```text
Subtask task-198-02-shockwave-ring-width (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-198-shockwave-ring-readable/task-198-02-shockwave-ring-width.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Why: in the task-197-07 `-game` frames the orange shockwave ring (24 uu) was hardly visible. Owner (2026-10-16) accepted: about 60 uu wide. The C++ timing change is done (task-198-01: grow 0.6 s, fade 0.2 s, outlives the sphere; see sys-explosions).
- What: in `/Game/RogueRigs/FX/M_ExplosionShockwave`, the width hard-coded at 24 in the progress-ring Custom node becomes a scalar parameter `RingWidth` (default 60 uu, provisional, owner 2026-10-16) wired into that node; keep the ring inner-aligned (Radius x Progress - RingWidth to Radius x Progress) so it is not cut at the decal box edge. `MI_ExplosionShockwave` keeps the default (read it back). Update the `look` and `parameters` lines of `bp-explosion-shockwave` (the cpp agent added a timing line: keep it).
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp; if the MCP tools are not exposed to you, JSON-RPC over HTTP works). One other bp agent works in parallel on another asset (task-198-02 M_ExplosionShockwave, task-199-01 NS_BarrelExplosion): touch only your own asset; never save all; save by explicit asset path; check `IsPIERunning` before saving.
- Never call a viewport or editor capture tool (bp-mcp-lessons). No PIE needed.
- Scope: exactly this. No cheat, no debug command, no new test.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; on an index.lock from the other agent, wait and retry. Run `node vault/scripts/lint.js --write` after editing the vault. Messages end with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets changed (paths), values read back, MCP tools that failed, docs, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
- Commit message prefix: `task-198-02-shockwave-ring-width: `.
```

#### task-199-01-barrel-burst-bigger (bp) — prompt sent
Reading list: the subtask node and its `inputs`.
```text
Subtask task-199-01-barrel-burst-bigger (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-199-barrel-burst-bigger/task-199-01-barrel-burst-bigger.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Why: in the task-197-07 `-game` frames the burst of `NS_BarrelExplosion` was small next to the 735 uu sphere: one orange glow, few visible sparks, smoke barely visible. Owner (2026-10-16) accepted: wider fireball, about twice the sparks, more visible smoke.
- What: edit `/Game/RogueRigs/FX/NS_BarrelExplosion` (and `M_BarrelExplosionSprite` only if smoke needs it): fireball sizes and spawn spread larger (fill more of the visual radius, still from `User.Radius`), sparks about 2× (about 56) and a little larger or brighter so they read from the top-down camera, smoke more opaque or larger or more puffs so it is visible on light grey rock and dark floor; everything ends before 1.5 s (`BurstSeconds`); colours per color-code (no red). All values provisional, owner 2026-10-16. Update `bp-barrel-explosion` with the new values and the reason.
- Editor state: open (launched by the Orchestrator; MCP at http://127.0.0.1:8000/mcp; if the MCP tools are not exposed to you, JSON-RPC over HTTP works). One other bp agent works in parallel on another asset (task-198-02 M_ExplosionShockwave, task-199-01 NS_BarrelExplosion): touch only your own asset; never save all; save by explicit asset path; check `IsPIERunning` before saving.
- Never call a viewport or editor capture tool (bp-mcp-lessons). No PIE needed.
- Scope: exactly this. No cheat, no debug command, no new test.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`; on an index.lock from the other agent, wait and retry. Run `node vault/scripts/lint.js --write` after editing the vault. Messages end with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: assets changed (paths), values read back, MCP tools that failed, docs, commit hash(es), what is unverified. Always end with a report, even partial; commit what works first.
- Commit message prefix: `task-199-01-barrel-burst-bigger: `.
```

#### task-198-02-shockwave-ring-width (bp) — result
- `M_ExplosionShockwave`: scalar parameter `RingWidth` (default 60 uu, provisional) wired into the progress-ring Custom node (input `W`), ring still inner-aligned; `MI_ExplosionShockwave` inherits 60. Doc `bp-explosion-shockwave` (look, parameters, derivation). One MCP call retried with the documented workaround (Custom node inputs). Commit `1edbf66e`. No second run.
- Unverified: look on screen (task-200 frames).

#### task-199-01-barrel-burst-bigger (bp) — result
- `NS_BarrelExplosion`: flash 1.2–1.6 R; fireball 16 (spawn 0.4 R, size 0.5–0.85 R); sparks 56 (0.03–0.06 R, colour 6, 3, 0.6); smoke 12 (spawn 0.4 R, 0.5–0.8 R, colour 0.15, 0.14, 0.13, alpha 0.85); 95 particles; ends about 1.4 s; no red. Provisional. Doc `bp-barrel-explosion`. Commit `f995b7b2`. No retry.
- Unverified: look and cost (task-200 frames for the look).

Editor closed by the Architect for task-200 (no tracked file modified).

#### task-200-01-decal-green-square-check (cpp) — prompt sent
Reading list: the subtask node and its `inputs`.
```text
Subtask task-200-01-decal-green-square-check (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-200-decal-green-square-check/task-200-01-decal-green-square-check.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- Why: in the task-197-07 `-game` run, the first frame of each ground decal (the barrel danger zone at fuse start, the explosion shockwave ring) showed a large pale green concrete-like square over the area; from the next frame the decal rendered normally. Likely the engine default decal material while decal shaders compile on first use. Owner (2026-10-16) accepted: a second run to confirm; nothing fixed if it is the shader compile.
- What: rerun the same check with existing routes only (no new or temporary command): the task-197-07 command line was
  UnrealEditor.exe "<uproject>" /Game/RogueRigs/Maps/L_ArenaM1 -game -windowed -ResX=1280 -ResY=720 -nosplash -log -ExecCmds="rr.Stress.Peace 1, rr.Debug.Delayed 6 ke BP_Miner_C K2_AddActorWorldOffset '(X=1608,Y=469,Z=0)' false () true, rr.Debug.Shots 8 1 1 barrelfx_pre, rr.Debug.Delayed 9.9 ke BP_ArenaGrid_C ApplyShotDamage '(X=145,Y=124)' 100, rr.Debug.Shots 9.8 18 0.15 barrelfx, rr.Debug.Delayed 14 quit"
  Use a new prefix (for example `barrelfx2`) so the old frames stay. Run it once; if the green square appears again, run it a second time to see whether it persists once shaders are cached (two runs at most). Search the log for shader compile lines around those moments (for example `ShaderCompil`, `Compiling`, `DefaultDeferredDecalMaterial`).
- The same frames also show what changed since task-197-07 (no extra work): the shockwave ring now grows over 0.6 s after the sphere and is 60 uu wide (task-198); the Niagara burst is bigger (task-199, 95 particles). Describe them as seen.
- Look at the frames yourself (Read the PNGs) and describe each relevant one: fuse start, fuse, explosion, after the sphere (ring visible?), burst size, smoke.
- Editor state: closed (the Orchestrator closed it). If an editor appears, stop and report; never kill it. No build needed unless the binaries are out of date (C++ changed in task-198-01: if `-game` refuses or is stale, build first per build-toolchain).
- Git: commit nothing (screenshots stay local). Do not modify the task node fields status, model, tokens, runs, result.
- Report: command line(s), frame paths with one line each of what is visible, whether the green square appeared (run 1, run 2), log lines about shader compilation, anything that looks wrong.
```

#### task-200-01-decal-green-square-check (cpp) — result
- Two `-game` runs (prefixes `barrelfx2`, `barrelfx3`), same command as task-197-07; binaries up to date; nothing built or committed.
- The green square returns in both runs, on the first frame of each new decal (fuse start, explosion), with a warm shader cache and no shader compile line in the logs: **not** the shader compile. Agent's best candidate (unverified): the renderer draws the engine default decal material (a green concrete texture) while a just-created dynamic material instance is not ready on its first rendered frame (`DecalRenderingShared.cpp` `GetIncompleteMaterialWithFallback`); the green rectangle matches the decal box (700–735 uu). On the explosion frame the newly opened crater is also drawn black for one frame and the sphere is not drawn yet.
- What the Architect saw (frames `barrelfx2_09` to `_12`): 09 green box and black crater; 10 orange sphere with a clear yellow 60 uu ring inside, many sparks, two fireball glows; 11 sphere gone, the orange-brown ring still visible near full radius, grey smoke haze; 12 ring gone, haze and glows fading. The ring and the bigger burst now read well.
- Decision: subtask done (report only, `done_when` met). The condition the owner set ("nothing fixed if it is the shader compile") does not hold: the next step is the owner's decision, asked with a default.

#### Owner answer on the green square (verbatim)
> Oui corriger les points

English translation: "Yes, fix the points."

Decision: both points fixed, the proposal (warm the decal materials at level load, treat the black crater, verify with the same `-game` run) is the plan, confirmed by this answer. Two owner points = two global tasks (split rule), run one after the other (both cpp, one build at a time, editor closed):

| Task | Agent | Goal |
|---|---|---|
| `task-201-decal-first-frame-green` | cpp | no default-material (green) frame when a ground telegraph or an explosion shockwave decal appears: materials ready before first use (warm-up at level load, or pooled decal components created early); game rules unchanged; verified with the task-200 `-game` command, frames at fuse start and explosion |
| `task-202-crater-first-frame-black` | cpp | no black crater on the frame an explosion opens it: find why newly exposed floor renders black for one frame and fix it; game rules unchanged; verified the same way |

No new test, no cheat; frames are not committed.

#### task-201-01-decal-first-frame-green (cpp) — prompt sent
Reading list: the subtask node and its `inputs`.
```text
Subtask task-201-01-decal-first-frame-green (Rogue Rigs, repo C:\Users\soumo\Documents\Unreal Projects\RogueRigs).

Read your subtask node first: vault/reference/tasks/task-201-decal-first-frame-green/task-201-01-decal-first-frame-green.md. Its `inputs` line lists the resolved paths of every node you need: read those directly, do not descend from root again, never search the vault. Resolve any other [[wikilink]] through vault/_paths.md.

Notes:
- The defect (task-200 report, two `-game` runs, warm shader cache, no shader compile in the logs): on the first rendered frame of each new decal, a large pale green concrete-textured rectangle the size of the decal box is drawn: at the barrel fuse start (ground telegraph, `URRGroundTelegraphComponent`, created on demand by the grid) and at the explosion (shockwave decal created by `ARRExplosionFX::CreateShockwave`). From the next frame the decal renders normally. Frames: `Saved/Screenshots/WindowsEditor/barrelfx2_01.png`, `barrelfx2_09.png` (and `barrelfx3_*`). Previous agent's best candidate (unverified): the renderer's fallback to the engine default decal material while the material (a dynamic instance created that frame) is not ready on its first frame (`Renderer/Private/DecalRenderingShared.cpp` `GetIncompleteMaterialWithFallback`); PSO precache (`r.PSOPrecache.ProxyCreationStrategy`, `DecalComponent.cpp`) was judged less likely. Both components assign the material before `RegisterComponent`.
- Owner (2026-10-16): "fix the points". Find the real cause first (engine source is available under the UE_5.8 install; read it), then fix it in our code: for example make the decal materials ready before first use (warm-up or precache at level load, components or material instances created early and kept hidden, pooled shockwave decals), whatever the cause calls for. Game rules unchanged; telegraph and shockwave look unchanged once shown; no cost added per frame in steady state.
- Verify with the task-200 `-game` command and a new prefix (for example `barrelfx4`): UnrealEditor.exe "<uproject>" /Game/RogueRigs/Maps/L_ArenaM1 -game -windowed -ResX=1280 -ResY=720 -nosplash -log -ExecCmds="rr.Stress.Peace 1, rr.Debug.Delayed 6 ke BP_Miner_C K2_AddActorWorldOffset '(X=1608,Y=469,Z=0)' false () true, rr.Debug.Shots 8 1 1 barrelfx4_pre, rr.Debug.Delayed 9.9 ke BP_ArenaGrid_C ApplyShotDamage '(X=145,Y=124)' 100, rr.Debug.Shots 9.8 18 0.15 barrelfx4, rr.Debug.Delayed 14 quit". Look at the fuse-start and explosion frames yourself (Read the PNGs). A black crater on the explosion frame is a separate task (task-202): note it, do not fix it.
- Tests: run the explosion, barrel fuse, ground telegraph and integration explosion tests headless; update only expectations your change breaks; no new test. Known unrelated failures: `Ore.HealVein` (flaky), `Enemies.ShotHitsMovingRobotAndDeathShowsParts`, `Integration.JuiceHealRing`.
- Docs: write the cause and the fix in `sys-ground-telegraph` and `sys-explosions` (and `sys-grid-barrels` if the grid changes).
- Editor state: closed. Build from the command line per build-toolchain. If an editor appears and Live Coding blocks the build, stop and report; never kill it.
- Scope: exactly this. No cheat, no debug command committed, no new test. Screenshots not committed.
- Git: commit only your files, by explicit path (`git commit -m "..." -- <paths>`), never `git add -A` or `-- vault`. Messages `task-201-01-decal-first-frame-green: <summary>`, ending with the line `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Do not modify the task node fields status, model, tokens, runs, result.
- Report: the cause with evidence (engine file and line, log or frame), files changed, the fix, build and test results, the frame paths and what each fuse-start and explosion frame shows, commit hash(es), what is unverified.
```
