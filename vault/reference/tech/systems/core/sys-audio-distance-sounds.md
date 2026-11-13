---
id: sys-audio-distance-sounds
description: "FRRDistanceSound and FRRExplosionSound: distance fade of world sounds, one-shot or loop, extractor rotor loop; how to add a distance sound."
parent: systems-core-index
kind: doc
---
# Audio — distance sounds

- responsibility: every sound heard from a point of the world and faded by the player's distance goes through one C++ base class (task-522, commits 7ee67e99, 6408a143; owner Q7 2026-11-13, [[audio-direction]]). The table row holds the values, the class holds how the distance is measured and whether it plays once or loops.
- main_classes: `Audio/RRDistanceSound.h/.cpp`: `FRRDistanceSound` (plain C++, not a UObject) and its child `FRRExplosionSound`. `Audio/RRAudioLog.h`: the one `LogRRAudio` category (extern, defined in `RRAudioSubsystem.cpp`), shared by the sound events, the distance sounds and the music; a `DEFINE_LOG_CATEGORY_STATIC` per file collided in a unity build (task-524).
- entry_points: `FRRDistanceSound(Event, Variant).PlayAt(Context, Location)` (one-shot, built on the stack); a member `FRRDistanceSound` of the source actor with `UpdateLoop(Context, bOn, Location)` every tick and `StopLoop(Context)` at `EndPlay` (loop row). `SetPlaysWhilePaused(true)`: the loop plays as a UI-class play and the pause does not stop it (the holder releases it). `FRRExplosionSound(Event, Radius).PlayAt(...)`. Virtual `GetDistance(Player, Location)`: horizontal distance (explosion: minus `Radius`, floor 0). Static `GetFade(Row, Distance)`. `URRAudioSubsystem::PlayEventAt` is a point `FRRDistanceSound`.
- fade: `Volume` up to `FadeInner`, linear to silence at `FadeOuter`; a one-shot at or beyond `FadeOuter` is not played (`Skip` line). A loop starts (faded in over `LoopFadeIn`) once the player is inside `FadeOuter`, follows the source, sets its level from the distance on each call, and fades out over `LoopFadeOut` when released or when the player leaves `FadeOuter`. A paused game stops a loop at once unless it plays while paused; the next update restarts it. Row values: [[sys-audio]] `distance_fade`.
- users: `Impact.Wall` (`ARRProjectile`), `EnemyDeath` (`ARREnemy`), `Explosion` (`URRExplosionSubsystem`, explosion child with the `ExplosionStats.csv` `Radius`), `ExtractorRotors` (`ARRShuttle`, loop), `ObjectiveComplete` (`ARRConsole::CompleteHack`, one-shot at the actor location = zone centre, console and chest; task-527), `rr.Audio.Play` with an offset (explosion child when an edge radius is given). Levels at 0, 200, 600, 1075 uu unchanged by the move (checked in `-game`).
- rotor_loop: `ARRShuttle::RotorSoundEvent` (EditDefaultsOnly, default `ExtractorRotors`; None = silent) and `RotorSound`; `TickRotorSound` after `TickRotors`: one loop at the actor location while the rotors spin, pause-proof from `StartTakeoff`, released at `EndPlay` ([[sys-objectives-shuttle]]). Row `ExtractorRotors`: `SW_ExtractorRotors` (owner ElevenLabs file, 4.38 s loop, placeholder), `Volume` 1.0, `bSpatial`, `FadeInner` 800, `FadeOuter` 2150 (Q4, Q5 of [[task-522-audio-distance-sound-rotors]], provisional), `LoopFadeIn` 0.15, `LoopFadeOut` 0.3 (Executor values, provisional). Checked in `-game`: one play per extractor, arrival at 543 uu factor 1.00, extraction 1500 uu 0.48, 900 uu 0.93, takeoff behind the stats screen at 1.00.
- add_a_distance_sound: add the row with `bSpatial`, `FadeInner`, `FadeOuter` (and `bLoop`, fades for a loop) per [[sys-audio]] `add_a_sound`; one-shot: `FRRDistanceSound(TEXT("Event")).PlayAt(this, Location)` at its moment; loop: an `FRRDistanceSound` member set in `BeginPlay`, `UpdateLoop` in `Tick`, `StopLoop` in `EndPlay`. A sound measured otherwise (from a zone edge, a segment): a child class overriding `GetDistance` (and `GetEdgeRadius` for the logs).
- debug: `log LogRRAudio Verbose`: `Play` and `Skip` lines with distance and fade; loops: `Loop <event>: <d> uu ..., fade <f>` at start and each time the factor crosses a 0.1 step, `Loop stop <event> (<reason>)`, `Loop <event> ended`.
- depends_on: [[sys-audio]] (event table, subsystem plays).
- implements: [[audio-direction]]
- gotchas: the pause-proof restart at the start of the extraction takeoff gives a 0.15 s fade-in dip (the play is started again as a UI-class play). A destroyed extractor still in hearing range fades out over 0.3 s. Not heard by the agent: the owner judges levels by ear.

## Relations
- [[systems-core-index]] — parent index.
- [[sys-audio]] — the event table and the subsystem these classes play through.
- [[audio-direction]] — the distance rules they apply.
- [[sys-objectives-shuttle]] — the extractor that holds the rotor loop.
- [[task-522-audio-distance-sound-rotors]] — the task that built them.
- [[task-527-audio-zone-objective-complete]] — the objective complete one-shot, a user of the base class.
