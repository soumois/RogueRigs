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
