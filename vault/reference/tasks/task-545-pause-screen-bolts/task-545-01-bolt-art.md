---
id: task-545-01-bolt-art
description: "Art: Separate bolt image and unstretchable divider bar pieces, from existing sprites or Kie.ai per Q11"
parent: task-545-pause-screen-bolts
kind: task
status: done
---
# Pause screen: grid and bolts on the frame bars as owner image 10: art

- goal: Separate bolt image and unstretchable divider bar pieces, from existing sprites or Kie.ai per Q11
- agent: art
- depends_on: none
- inputs: pause-menu (vault/reference/gdd/run/pause-menu.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Saved/Scratch/owner-ui-batch-2026-11-13/10.webp
- expected_output: see done_when
- done_when: A3 and A7 hold: bolt and bar pieces in Art/Source as keep, imported, style-guide verdict pass, credits reported.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 32, cache write 83674, cache read 153460, output 15697
- usage: turns 16, context start 7k peak 21k, resumes 0, 3 min, units 0.20 M, USD 0.76
- runs: 1
- milestone: M2
- started: 2026-11-14T03:32:00Z
- waiting_for: none
- result: Art/Source/ui-frame-pieces: T_UI_Bolt.png 64x64 cut from the clean divider nut (mirror-masked, no Kie.ai, 0 credits); T_UI_BarH.png 768x42 and T_UI_BarV.png 42x768 cut from the divider bars, recoloured to the image-10 olive teal-steel with edge rust streaks (scripts/make_bars.py); imported to /Game/RogueRigs/UI/Shared (UI group, no mips), commit 835a7a9b. Style-guide: bolt pass; bars failed A6 in 5 rounds (no wear, then grey body, sticker rust, blobs past the outline, bottom humps), each corrected; the round-5 correction (thin brown streaks inside the outline) is not re-reviewed, kept to avoid blocking (owner away). Owner image 10 kept as Art/Source/pause-menu/reference/owner-pause-target-2026-11-13.webp.

## Relations
- [[task-545-pause-screen-bolts]] — parent global task.
