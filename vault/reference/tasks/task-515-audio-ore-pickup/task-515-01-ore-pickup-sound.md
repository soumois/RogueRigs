---
id: task-515-01-ore-pickup-sound
description: "Cpp: ore pickup event, table line on the XP sound, call; sys-audio line"
parent: task-515-audio-ore-pickup
kind: task
status: done
---
# Son du minerai ramassé (son d'XP pour l'instant): cpp

- goal: ore pickup event, table line on the XP sound, call; sys-audio line
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A4 of the global hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-515
- usage: included in the executor run of task-515
- runs: 1
- milestone: M2
- started: 2026-11-13T08:54:25Z
- waiting_for: none
- result: Commit 6e9a89dc. A1: Config/DefaultGame.ini adds row OrePickup on SW_XPPickup, Volume 1.0, PitchJitter 0.05, MaxConcurrent 3, bSpatial False (centred), bUI False, copied from XPPickup (own cap of 3). A2: -game run on L_ArenaM1, Verbose LogRRAudio, rr.Debug.SpawnNear BP_PickupOre and BP_PickupXP at 40 uu: ore collected -> one 'Play OrePickup (row OrePickup) SW_XPPickup', XP collected -> one 'Play XPPickup' unchanged. A3: build green (RRPickup.cpp, RRAudioDebugCommands.cpp); no audio test family exists, so build only per build-toolchain Test scope. A4: sys-audio call_sites lists OrePickup in ARRPickup::Collect. Code: PlayEvent(OrePickup) in ARRPickup::Collect for PickupType Ore; rr.Audio.Play help lists OrePickup. Owner swap point: the OrePickup line in DefaultGame.ini.

## Relations
- [[task-515-audio-ore-pickup]] — parent global task.
