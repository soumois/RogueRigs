---
id: task-510-05-options-sound-classes
description: "Bp: sound classes Master, Music and Effects (Effects and Music children of Master) driven by the settings, existing sound assets assigned"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: bp

- goal: sound classes Master, Music and Effects (Effects and Music children of Master) driven by the settings, existing sound assets assigned
- agent: bp
- depends_on: task-510-04-options-widget-cpp
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: see done_when
- done_when: The three sound classes exist and are the ones the settings drive; every existing sound asset belongs to Music or Effects; saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:22:55Z
- waiting_for: none
- result: 8fb28154. /Game/RogueRigs/Audio/SC_Master (no parent, children SC_Music and SC_Effects), SC_Music and SC_Effects (parent SC_Master), duplicated from the engine Master, Music and SFX classes and relinked through MCP (AssetTools.duplicate, ObjectTools.set_properties), saved. They are the paths URROptionsSettings defaults to. The project has no sound asset yet (find_assets: 0 SoundWave, SoundCue, MetaSoundSource under /Game), so none to assign; sounds added later (M3) take SC_Music or SC_Effects.

## Relations
- [[task-510-options-screen]] — parent global task.
