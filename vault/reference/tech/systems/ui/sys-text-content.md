---
id: sys-text-content
description: "Text loader as built: URRTextContentSubsystem reads the A4 JSON text files, serves upgrade flavor, alert lines and barks with fallback; hooks and tests."
parent: systems-ui-index
kind: doc
---
# Text content loader

- responsibility: read the validated text files of [[text-content-schema]] at runtime (no AI at runtime) and serve them to C++ callers and the Blueprint layer; every missing piece falls back to the functional text (task-079-01).
- main_classes (`Source/RogueRigs/Text/`): `FRRTextContentLibrary` (plain class: parses the three files, closed id lists of [[text-content-events]], picks); `URRTextContentSubsystem` (`UWorldSubsystem`, Game and PIE worlds, loads lazily on the first lookup, once per world).
- entry_points: `GetUpgradeFlavor(StatNameOrId, OutText)` (display name "Fire rate" or id "upg_fire_rate"); `GetAlertLine(EventId, OutText)` picks a line and stores it as current; `GetCurrentAlertLine(EventId)` (BlueprintPure); `GetBark(TriggerId, SpeakerId, OutText)` (None = any speaker); `SetRandomSeed`; `ReloadFromDirectory` (tests); static `Get(WorldContext)`, `ComposeUpgradeDescription`.
- hooks: `URRUpgradeComponent::MakeCard` sets a stat card `DescriptionText` = stat name, newline, flavor ([[sys-upgrades]]); the five alert lines are picked through `URRSubtitleSubsystem::ShowAlertLine` and shown as Kessler-Voss subtitles (task-099-01, [[sys-subtitles]]): `URRRunFlowSubsystem` for `console_dropped`, `bonus_chest_dropped`, `extraction_ready` just before broadcasting them and `shuttle_countdown` once in `LandShuttle`, before the first countdown event; `URRHUDWidget::HandleHordeIncoming` for `horde_incoming` before `OnHordeIncoming` ([[sys-hud-pressure]]). `GetAlertLine` / `GetCurrentAlertLine` keep working (they store the current line). Barks: lookup only, nothing displays them.
- picks: uniform among the event's lines, never the previous pick of that event when it has more than one (`PickIndexAvoiding`); stream seeded from `FMath::Rand` at `Initialize`.
- data: `Content/RogueRigs/Data/Text/upgrade_texts.json`, `alerts.json`, `barks.json`. Packaged builds: already staged by `+DirectoriesToAlwaysStageAsNonUFS=(Path="RogueRigs/Data")` in `Config/DefaultGame.ini` (recursive; no change needed).
- fallback: a file that is missing, unparsable, not `schema_version` 1, with a `run_id` not matching `<date>-<time>-<stem>` (a `-noretrieval` baseline is refused) or without items loads nothing: one warning, lookups return false and an empty text. A malformed item or an id outside the closed lists is skipped (one warning). An unknown id at lookup warns once per id. Never a crash.
- depends_on: [[sys-upgrades]], [[sys-objectives]], [[sys-hud-pressure]].
- implements: [[assignment-4-plan]] loader (content types 1 to 3).
- gotchas: the C++ id lists mirror [[text-content-events]]: adding an id needs both. Alert lines are displayed by the subtitle widget ([[sys-subtitles]]); `GetCurrentAlertLine` / `URRHUDWidget::GetAlertLine` remain for tests and Blueprints. Tests: `RogueRigs.Text.*` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[text-content-schema]] — the file contract it reads.
- [[text-content-events]] — the id lists it enforces.
- [[sys-upgrades]] — card description hook.
- [[sys-hud-pressure]] — alert event hooks.
- [[sys-subtitles]] — shows the picked alert lines as subtitles.
