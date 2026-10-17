---
id: run-log-schema
description: "Run log format and field names: one JSON file per run in Saved/RunLogs, one entry per level with 11 snake_case fields; feeds the balance agent."
parent: pipelines-index
kind: doc
---
# Run log schema

Format (owner 2026-10-17, task-212): JSON, UTF-8, one file per run `Saved/RunLogs/<yyyyMMdd-HHmmss>.json` (local start time; `_2`… on a clash). Created at the run start, rewritten after each level entry. A run ends on run won, death or missed shuttle; a restart starts a new file. Writer: [[sys-run-log-writer]].

Top level:
- `run_id` (the file name stem), `start_time`, `end_time` (UTC ISO 8601; `end_time` null while in progress);
- `build_version` (engine build string), `build_configuration`;
- `layouts`: layout name of each level started, in order;
- `outcome`: `in_progress`, `won`, `death`, `missed_shuttle`, `abandoned` (restart before an end);
- `levels`: one entry per ended level, in order.

Level entry (`level_index`, `layout_name`, then the 11 fields):
1. `terrain_destroyed`: `by_source` (barrel, kamikaze, drone, shot) and `by_type` (rock, ore, heal_ore, special_ore, barrel), block counts.
2. `damage_dealt_by_weapon`: weapon class → raw damage to robots.
3. `kills`: `by_robot_type` (EnemyStats row) and `by_source` (weapon, explosion, other).
4. `upgrades_picked`: `order` (card ids in pick order) and `frequency` (card id → count).
5. `time_per_level`: seconds of world time from level start (end of the loading hold after a travel) to level end; pauses (level-up screens, end pause) excluded.
6. `damage_taken_by_source`: robot type, explosion or other → raw damage.
7. `xp_collected`: XP gained (after XPGain).
8. `level_outcome`: `extracted`, `death`, `missed_shuttle`.
9. `time_left_at_shuttle`: countdown seconds left when the miner boarded; null when the shuttle was not reached.
10. `ore_collected_in_risk_window`: carried ore gained between the shuttle landing and the level end.
11. `events_met`: list of `{event, index?, outcome}` for the alert events that happened: `console` (hacked / not_hacked), `bonus_chest` (opened / not_opened), `horde` with its `index` (lull / announced / spawning / done).

Key names of the collected counters: [[sys-run-log]].

GAS already writes to the Visual Logger; no custom GAS logging in M1 ([[gas-scope]]).

## Relations
- [[pipelines-index]] — parent index.
- [[destruction-rules]] — destruction sources.
- [[sys-run-log]] — the collector of the counters.
- [[sys-run-log-writer]] — the code that writes this format.
