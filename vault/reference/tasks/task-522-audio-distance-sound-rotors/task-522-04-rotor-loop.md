---
id: task-522-04-rotor-loop
description: "Cpp: extractor plays one rotor loop with distance per Q4, Q5"
parent: task-522-audio-distance-sound-rotors
kind: task
status: done
---
# Son des hélices de l'extracteur et code générique de son à distance: cpp

- goal: extractor plays one rotor loop with distance per Q4, Q5
- agent: cpp
- depends_on: task-522-03-sound-import
- inputs: sys-audio (vault/reference/tech/systems/core/sys-audio.md), extraction (vault/reference/gdd/run/extraction.md), bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), Config/DefaultGame.ini
- expected_output: see done_when
- done_when: A3, A4 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-522
- usage: included in the executor run of task-522
- runs: 1
- milestone: M2
- started: 2026-11-13T14:28:44Z
- waiting_for: none
- result: Done (6408a143). A3: ARRShuttle holds one FRRDistanceSound RotorSound (RotorSoundEvent ExtractorRotors) updated each tick after TickRotors at the actor location, on while the rotors spin, released at EndPlay (0.3 s fade); pause-proof (SetPlaysWhilePaused, UI-class play) once taking off. Row ExtractorRotors: SW_ExtractorRotors, Volume 1.0, bSpatial, FadeInner 800, FadeOuter 2150, bLoop, LoopFadeIn 0.15, LoopFadeOut 0.3 (fades: executor values). -game Verbose LogRRAudio: arrival one play at 543 uu fade 1.00, released when out of view (2.58 s); extraction no play while beyond 2150 uu, one play at 1500 uu fade 0.48, 900 uu 0.93, then boarding: paused takeoff stop+restart pause-proof at 303 uu fade 1.00, released out of view (3.08 s). Never more than one play per extractor. Build fix asked by the coordinator: LogRRAudio declared once in Audio/RRAudioLog.h (DEFINE_LOG_CATEGORY in RRAudioSubsystem.cpp), the task-524 LogRRMusic workaround in RRMusicSubsystem.cpp replaced by it; built with RRDistanceSound.cpp and RRMusicSubsystem.cpp in the same unity file (Module.RogueRigs.2), green. A4 code: build green; no audio test family; the C4996 Cast warning of unity Module.RogueRigs.5 (tests) predates this task (seen on a stash build). Owner retouch: the ExtractorRotors line of Config/DefaultGame.ini. Known: a 0.15 s fade-in dip at the paused takeoff start (play restarted as pause-proof).

## Relations
- [[task-522-audio-distance-sound-rotors]] — parent global task.
