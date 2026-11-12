---
id: task-510-06-options-sprites-import
description: "Bp: import the new options PNGs as UI textures"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: bp

- goal: import the new options PNGs as UI textures
- agent: bp
- depends_on: task-510-05-options-sound-classes
- inputs: options-screen (vault/reference/gdd/run/options-screen.md)
- expected_output: see done_when
- done_when: Every new PNG of task-510-02 imported as a UI texture with settings matching the existing menu textures; saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:32:59Z
- waiting_for: none
- result: a7808a69. /Game/RogueRigs/UI/Options: T_Options_Tab 1024x199, SelectorPlate 1024x136, Arrow 256x234, SliderTrack 1024x78, SliderFill 1024x75, SliderKnob 96x128, ToggleOn 512x123, ToggleOff 512x125, ToggleKnob 256x254, imported with TextureTools.import_file from Art/UI/Options; settings as T_Shop_ButtonContinue (TEXTUREGROUP_UI, TC_EditorIcon, no mipmaps, sRGB, never stream), read back; saved.

## Relations
- [[task-510-options-screen]] — parent global task.
