---
id: task-222-01-shield-debris-pieces
description: "3D agent: cut the 1.2-scaled shield into five debris meshes (four plate shards, grip) with centred pivots and an offsets json."
parent: task-222-shield-break-debris
kind: task
status: done
---
# Shield debris pieces

- goal: `Art/Source/shield-chaser/debris/SM_ShieldDebris_*.fbx` and `shield_debris_pieces.json`, ready for a Niagara mesh renderer.
- agent: agent-3d (Blender, no Meshy)
- depends_on: none
- inputs: `Art/Meshes/ChaserShield/SM_ChaserShield.fbx` and report (979 tris, two slots: shield, emblem decal), `Art/Meshes/ChaserShield/T_ChaserShield_BaseColor.png`, `T_ChaserShield_Emblem.png`; `Tools/art/blender/chaser_debris_pieces.py` (format to copy: rest pose, cut caps, pivot at bounds centre, UV0 only, `debris_pieces.json` with `centre_offset_cm_unreal` = (x, -y, z)); `Tools/art/blender/shield_chaser_bake.py` (the shield scale 1.2 and its clip 0 frame 0 placement); `vault/reference/tech/pipelines/chaser-meshy-pipeline.md` (step 6, debris), `vault/reference/agents/art-3d-lessons.md`.
- method: script `Tools/art/blender/shield_debris_pieces.py`. Shield scaled 1.2 (98.4 x 51.4 cm). Cut into Shard_A..Shard_D (irregular straight cuts, chunky low-poly look, the emblem decal stays on its shard) and Grip (the dark handle and mounts). Cap the cuts with the shield yellow (cap faces use a yellow texel of the base colour UV). Offsets: from the actor origin in the guard pose (clip 0 frame 0: where the shield is when it breaks), Unreal axes, so the Niagara system can place each piece where it was. Two slots per shard that holds the emblem (shield, emblem), one otherwise; UV0 only; centimetres, FBX_SCALE_UNITS.
- done_when: 5 FBX + json (triangle count per piece, offsets); a render of the five pieces exploded slightly apart and one of them reassembled, three-quarter front view; the Orchestrator opens them.
- editor_state: any (no Unreal)
- token_ceiling: open
- model: sonnet
- tokens: input 64, cache write 106380, cache read 2120199, output 10733
- runs: 1
- milestone: M1
- result: Commit 5c2f3c01: Tools/art/blender/shield_debris_pieces.py; Art/Source/shield-chaser/debris/: SM_ShieldDebris_Shard_A (240 tris, KV emblem, slots M_ChaserShield + M_ChaserShield_Emblem), Shard_B 116, Shard_C 132, Shard_D 46, Grip 721 (1255 tris with cut caps); shield_debris_pieces.json with centre_offset_cm_unreal from the actor origin in the guard pose (no spawn rotation needed); exploded and reassembled renders. Grip keeps 9 open edges from the Meshy grip.

## Relations
- [[task-222-shield-break-debris]] — parent global task.
