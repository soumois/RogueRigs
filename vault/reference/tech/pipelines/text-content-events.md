---
id: text-content-events
description: "Closed lists of bark trigger ids and alert event ids, each mapped to its existing C++ source event. Open before writing barks or alerts."
parent: text-content-schema
kind: doc
---
# Text content events

Only these ids are valid; an id not listed fails. Each maps to an event that already exists ([[sys-hud]], [[sys-hud-pressure]], [[sys-objectives]]); adding an id needs the event first.

## Bark triggers (`trigger` in barks.json)
| id | source event |
|---|---|
| `level_up` | `URRHUDWidget::OnLevelUp` |
| `upgrade_offer` | `URRHUDWidget::OnUpgradeOfferShown` (also after a reroll) |
| `player_damaged` | `URRHUDWidget::OnPlayerDamaged` |
| `horde_incoming` | `URRHUDWidget::OnHordeIncoming` |
| `console_dropped` | `URRRunFlowSubsystem::OnConsoleDropped` |
| `console_hack_started` | `ARRConsole::OnHackStateChanged` to Hacking (again on each re-entry) |
| `console_hacked` | `ARRConsole::OnHackStateChanged` to Complete |
| `bonus_chest_dropped` | `URRRunFlowSubsystem::OnBonusChestDropped` |
| `bonus_chest_opened` | `ARRBonusChest::OnRewardGranted` |
| `extraction_ready` | `URRRunFlowSubsystem::OnExtractionReady` |
| `shuttle_landed` | `URRRunFlowSubsystem::OnShuttleLanded` |
| `run_won` | `URRRunFlowSubsystem::OnRunEnded` (true; `ARRShuttle::OnExtracted`) |
| `run_lost` | `URRRunFlowSubsystem::OnRunEnded` (false; `ARRShuttle::OnMissed`) |
| `game_over` | `URRHUDWidget::OnGameOver` (miner death) |

## Alert events (`event` in alerts.json)
The alert-bar subjects of [[sys-hud-pressure]] (`ERRAlertBarEventType`) plus the shuttle countdown.
| id | source event |
|---|---|
| `console_dropped` | `URRRunFlowSubsystem::OnConsoleDropped` (alert-bar `Console`) |
| `bonus_chest_dropped` | `URRRunFlowSubsystem::OnBonusChestDropped` (`BonusChest`) |
| `horde_incoming` | `URRHUDWidget::OnHordeIncoming` (`Horde`) |
| `extraction_ready` | `URRRunFlowSubsystem::OnExtractionReady` (`Extraction`) |
| `shuttle_countdown` | `URRRunFlowSubsystem::OnShuttleCountdown` (every countdown frame; the loader picks one line per countdown start, [[sys-text-content]]) |

## Relations
- [[text-content-schema]] — parent: the pipeline contract.
- [[text-content-files]] — the files that use these ids.
- [[sys-objectives]] — run-flow, console, chest and shuttle events.
- [[sys-hud-pressure]] — damage, horde and alert-bar events.
