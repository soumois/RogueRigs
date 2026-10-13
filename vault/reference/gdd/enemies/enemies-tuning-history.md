---
id: enemies-tuning-history
description: "Dated owner playtest decisions on robots: recycling, digging toward robots, kamikaze share, level 1 pressure. Open for why a spawn value is what it is."
parent: enemies-index
kind: note
---
# Robot tuning history

Robot recycling (owner 2026-10-06 third test, provisional): a robot off screen, more than 2500 uu from the miner for 4 s, is removed and frees its slot, so the alive cap never starves new spawns ahead of a running miner (seen at 2–3 min in the third test).

Fourth test (owner 2026-10-06 fourth test, provisional): every robot uses the chaser capsule against rock (render and hits on the miner unchanged) so none gets stuck; robots no longer block each other, a light separation keeps them from stacking; robots left behind are removed once off screen and more than 1800 uu away for 3 s; packs hold 1–2 kamikazes, and at most 1 shooter in one pack out of two, never before 1:30, at most 4 shooters alive; packs hold 6–10 robots until 1:30.

Recycling by relocation (owner 2026-10-06, provisional): a robot off screen and more than 1800 uu from the miner is not removed but moved back into the spawn band ahead of the miner (heading split), keeping its type and health, so the pressure stays; it qualifies 3 s after leaving the screen if it has been on screen, 6 s after spawning if never seen; robots near the camera are never relocated.

Digging toward robots (owner 2026-10-07 fifth test, provisional): in the front sector the detour filter does not apply, so robots spawn beyond the rock the miner is digging toward and walk around to wait at the exit; the filter stays on the sides and behind.

Kamikaze share (owner 2026-10-07: at least twice too many, provisional): at most 1 kamikaze per pack, in one pack out of two on average; hordes carry 1–2 kamikazes (was 3–5).

First level too hard (owner 2026-10-07, provisional): about 15 % fewer robots: lone chaser every 2.35 s (was 2), wave interval 7.1 → 4.1 s (was 6 → 3.5), hordes 34–51 robots (was 40–60), overtime unchanged; heavies rarer: heavy pack weight 4 (was 8), 2–3 heavies per horde (was 3–5), at most 3 alive (was 5).

Level 1 pressure (owner 2026-10-07, provisional; the earlier values become the last-level profile): at most 80 robots alive (was 150); wave interval ends at 5.5 s (was 4.1); waves of 2–3 packs still from 1:30, but the packs of one wave spawn at least 120° apart around the miner.

## Relations
- [[enemies-index]] — parent hub: summary and verdict.

