---
id: task-510-03-options-settings-cpp
description: "Cpp: settings model: load, save and apply audio volumes, display and quality, gameplay toggles wired to damage numbers, camera shakes and the aim laser"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: cpp

- goal: settings model: load, save and apply sound volumes, display (window mode, resolution, VSync, frame limit, resolution scale), quality preset and its five sub-settings, the four gameplay toggles (damage numbers, aim indicator, screen shake, pause on focus loss); keep-or-revert of display changes
- agent: cpp
- depends_on: task-510-02-options-elements
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/combat/sys-damage-numbers.md), miner (vault/reference/gdd/player/miner.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), audio-direction (vault/reference/gdd/art-audio/audio-direction.md)
- expected_output: see done_when
- done_when: Build clean; each setting of options-screen has its default, applies at once, is saved to disk per Windows user at each change and applied at launch (first launch keeps current values); after a window mode or resolution change the previous display values stay saved until Keep, and Revert or the 15 s timeout restores them; the four gameplay toggles take effect in game; a debug console command sets a value for the capture; existing tests of the touched families pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-510
- usage: included in the executor run of task-510
- runs: 1
- milestone: M3
- started: 2026-11-12T09:16:05Z
- waiting_for: none
- result: 75dfba40. URRGameUserSettings extended (volumes Master/Music/Effects step 5 %, VSync, frame limit, resolution scale 50-100 step 5, quality preset Low..Epic/Custom over Shadows, Effects, Anti-aliasing, Post-processing, Textures, toggles DamageNumbers, AimIndicator, ScreenShake, PauseOnFocusLoss; every setter applies and saves GameUserSettings.ini; SaveSettings keeps the confirmed window mode and resolution while a display change is pending). New URROptionsSettings (sound class paths /Game/RogueRigs/Audio/SC_Master|SC_Music|SC_Effects, steps, frame limits, 15 s), URROptionsSubsystem (transient sound mix overrides, SetWindowMode/SetResolution with real-time keep-or-revert countdown, Keep/Revert, focus loss opens the pause menu), aim laser hidden and every camera shake skipped when off. Debug: rr.Options.Set/Dump/Keep/Revert. Build clean (only the pre-existing C4996 of RRGroundTelegraphTests); RogueRigs.DamageNumbers (10) and RogueRigs.Pickups.OreNumber pass. In-game effect and relaunch read-back left to task-510-10 C6. First launch: no file, engine values kept.

## Relations
- [[task-510-options-screen]] — parent global task.
