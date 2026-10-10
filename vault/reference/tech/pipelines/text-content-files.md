---
id: text-content-files
description: "Item fields, allowed values, word limits and one example each for upgrade_texts.json, barks.json and alerts.json. Open before writing or validating them."
parent: text-content-schema
kind: doc
---
# Text content files

Envelope, text rules and run id: [[text-content-schema]]. Examples below are reference only, never reused as content (T8).

## upgrade_texts.json (one line per stat)
| key | type | rule |
|---|---|---|
| `id` | string | `upg_` + `stat` lowercased, spaces replaced by `_` (`Fire rate` → `upg_fire_rate`) |
| `stat` | string | exactly one display stat name of [[sys-upgrades]] (Display), unchanged, case included; each stat at most once |
| `flavor` | string | 3 to 12 words (provisional schema choice), no digit |

```json
{"schema_version": 1, "run_id": "20261011-143000-upgrade_texts", "items": [
  {"id": "upg_reload_speed", "stat": "Reload speed", "flavor": "Swap the mag before the robots finish their paperwork."}]}
```

## barks.json (Rook and Vesna)
| key | type | rule |
|---|---|---|
| `line_id` | string | `<speaker_id>_<trigger>_<NN>`, NN two digits from `01` |
| `speaker_id` | string | `rook` or `vesna` ([[crew-characters]]) |
| `trigger` | string | one trigger id of [[text-content-events]] |
| `tone` | string | `cocky`, `deadpan`, `technical`, `propaganda` or `bureaucratic` |
| `text` | string | 3 to 8 words ([[dialogue-tone]] T1) |

```json
{"schema_version": 1, "run_id": "20261011-150000-barks", "items": [
  {"line_id": "vesna_console_dropped_01", "speaker_id": "vesna", "trigger": "console_dropped", "tone": "deadpan", "text": "Console is down. Walk, do not sightsee."}]}
```

## alerts.json (Kessler-Voss)
| key | type | rule |
|---|---|---|
| `line_id` | string | `kv_<event>_<NN>`, NN two digits from `01` |
| `event` | string | one alert event id of [[text-content-events]] |
| `register` | string | `propaganda` or `bureaucratic` ([[company-voice]]) |
| `text` | string | 3 to 12 words (provisional schema choice) |

```json
{"schema_version": 1, "run_id": "20261011-153000-alerts", "items": [
  {"line_id": "kv_horde_incoming_01", "event": "horde_incoming", "register": "bureaucratic", "text": "Asset defense surge authorized. Personnel on site are liable."}]}
```

## Relations
- [[text-content-schema]] — parent: envelope, text rules, run id.
- [[text-content-events]] — the trigger and event ids these files use.
