---
id: task-523-01-impact-curve
description: "Cpp: robot impact quieter near the player per Q9"
parent: task-523-audio-impact-near-quieter
kind: task
status: done
---
# Impact : un peu moins fort au corps à corps: cpp

- goal: robot impact quieter near the player per Q9
- agent: cpp
- depends_on: none
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A1 to A3 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-523
- usage: included in the executor run of task-523
- runs: 1
- milestone: M2
- started: 2026-11-13T14:38:34Z
- waiting_for: none
- result: Commit 1d206cbf: FRRSoundEventRow NearVolume/NearInner/NearOuter, FRRDistanceSound::GetFade = GetFarFade x GetNearFactor; row Impact NearVolume 0.6, NearInner 150, NearOuter 900. Build green (no audio test family exists: build only). A1 rr.Audio.Play Impact at 0/150/500/900/1075 uu logs fade 0.60/0.60/0.79/1.00/1.00 (near factor in the log). A2 Impact.Wall logs 1.00/1.00/0.66/0.20/skip at 1075, unchanged. Not done: the gdd_check line 'Edit: audio-direction' (a gdd node, outside the executor's rights and no subtask holds it).

## Relations
- [[task-523-audio-impact-near-quieter]] — parent global task.
