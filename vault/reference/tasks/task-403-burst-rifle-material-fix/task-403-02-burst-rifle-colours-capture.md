---
id: task-403-02-burst-rifle-colours-capture
description: "Capture: game capture: the rifle shows teal, rust and dark areas and is shaded like the miner"
parent: task-403-burst-rifle-material-fix
kind: task
status: todo
---
# Burst rifle material fix: colours readable in game: capture

- goal: A -game capture on the first arena checks the rifle colours against C1 to C3.
- agent: capture
- depends_on: task-403-01-burst-rifle-material-diagnose-fix
- inputs: agent-capture (vault/reference/agents/agent-capture.md), bp-burst-rifle (vault/reference/tech/blueprints/player/bp-burst-rifle.md), burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), Look before: `Saved/Captures/RifleInHand2/crop_still-b.png` (rifle flat cream-white beside the normally shaded miner), `Saved/Captures/RifleInHand2/contact_sheet.png`, Reference: `Art/Source/burst-rifle/meshy/burst_rifle_meshy_basecolor.png` (teal, rust, dark, cream atlas), Blender renders `Saved/BurstRifleMesh/flat/result_textured_three_quarter.png`, `result_textured_left.png`, `result_textured_right.png` (not committed, expected look), Framing of RifleInHand2: 1920x1080, camera arm 350 (see `vault/reference/tasks/task-400-burst-rifle-in-hand/task-400-06-rifle-in-hand-capture.md` result); debug command `rr.Debug.MinerAnimForce "x y"` if a pose change helps
- expected_output: Report C1..C3 seen, not seen or ambiguous with frame or crop paths; frames sent to the owner.
- done_when: C1 the rifle in the miner hand shows distinct teal, rust and dark (barrel, grip) areas, not the uniform cream-white silhouette of crop_still-b.png; C2 the rifle is shaded (lit and shadow sides differ) like the miner next to it; C3 the miner looks as in crop_still-b.png (no change). Each point reported; ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-403-burst-rifle-material-fix]] — parent global task.
