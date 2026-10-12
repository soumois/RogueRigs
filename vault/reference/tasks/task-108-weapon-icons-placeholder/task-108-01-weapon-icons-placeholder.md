---
id: task-108-01-weapon-icons-placeholder
description: "Placeholder UI icons (padlock and the three current weapons) exist as source images, validated by the owner, then prepared as transparent 256 x 256 PNGs."
parent: task-108-weapon-icons-placeholder
kind: task
status: in-progress
---
# Placeholder weapon and lock icons

- goal: Placeholder UI icons (padlock and the three current weapons) exist as source images, validated by the owner, then prepared as transparent 256 x 256 PNGs.
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), shotgun (vault/reference/gdd/weapons/shotgun.md), turret (vault/reference/gdd/weapons/turret.md)
- expected_output: candidate source images with the Kie.ai script (Tools/art/kie_image.js, GPT Image 2, resolution capped at 1K, 1:1): (1) a PADLOCK icon for the locked weapon slots (the owner wrote "looker"; read it as locker = padlock; if it meant something else it will be corrected on review); (2) one icon per current weapon: the burst rifle, the shotgun and the turret launcher (read their GDD nodes for the look; the game has no final weapon list yet, these are PLACEHOLDERS until the weapons are decided). Style: UI icon in the locked Rogue Rigs style (art-style-guide, color-code: player side white + teal + rust, player shots white-gold 1/0.9/0.5), a single object in side view facing right, flat shading, bold readable silhouette, centred with margin on a plain flat background of one colour that does not appear in the icon (so a later step can cut it out), no text, no shadow on the background, no hands or characters. 2 variants per icon (8 candidates at most), each self-reviewed against the A rules of art-style-guide, then reviewed by the style-guide agent per its spec; the owner validates and copies the winners to vault/assets/ (the agent never writes there). Write the run logs and prompts under Art/Source/weapon-icons/ as the agent spec says; add a prompt node under art-prompts only if the spec asks. No API key anywhere.
- done_when: candidates for the padlock and the three weapons exist with their run logs; self-review and style-guide review done; the owner validates; lint clean; committed (Art/Source is tracked).
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-108-weapon-icons-placeholder]] — parent global task.
