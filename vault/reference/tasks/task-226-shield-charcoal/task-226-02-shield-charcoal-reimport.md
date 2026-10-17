---
id: task-226-02-shield-charcoal-reimport
description: "cpp: headless reimport of T_ChaserShield_BaseColor and T_ChaserShield_Emblem, settings kept; then one -game capture of the charcoal shield."
parent: task-226-shield-charcoal
kind: task
status: in-progress
---
# Charcoal shield: reimport and capture

- goal: Unreal shows the charcoal shield and charcoal fragments.
- agent: agent-cpp (headless, no C++ change, no build)
- depends_on: task-226-01-shield-charcoal-textures
- inputs: `Tools/unreal/reimport_texture.py` and `vault/reference/tech/pipelines/tool-unreal-reimport.md` (companion scripts); assets `/Game/RogueRigs/Art/ShieldChaser/T_ChaserShield_BaseColor` and `T_ChaserShield_Emblem` (sources `Art/Meshes/ChaserShield/*.png`); `vault/reference/agents/agent-cpp.md`; the capture script of task-224-04 (`scratchpad\run4.ps1` in the session scratchpad) and `vault/reference/tech/systems/core/sys-cheats.md`.
- method: editor closed. Reimport both textures in place, read back size, sRGB, compression and that the emblem keeps its alpha. Then ONE game run: a shield chaser walking toward the miner (shot before the break, close crop), rr.Debug.BreakShields, a shot of the fragments about 1.5 s later. Save `vault/assets/shield-chaser-charcoal-ingame.png` and `-charcoal-debris.png`.
- done_when: read-back lines; images show a charcoal shield with yellow stripes and the yellow KV, and charcoal fragments; no error in the log; commit by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-226-shield-charcoal]] — parent global task.
