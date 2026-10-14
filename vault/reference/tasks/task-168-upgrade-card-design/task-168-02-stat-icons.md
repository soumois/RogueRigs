---
id: task-168-02-stat-icons
description: "Generate candidate icons for the 12 offerable stats (white flat silhouettes), show them to the owner, then turn the picked ones into transparent PNGs."
parent: task-168-upgrade-card-design
kind: task
status: done
---
# Upgrade cards: stat icons

- goal: Generate candidate icons for the 12 offerable stats (white flat silhouettes), show them to the owner, then turn the picked ones into transparent PNGs.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), upgrades (vault/reference/gdd/weapons/upgrades.md), sys-upgrades (vault/reference/tech/systems/core/sys-upgrades.md)
- expected_output: Candidates in Art/Source/stat-icons/ (2 variants per stat, flat magenta background, 1:1); after the owner pick: 12 transparent 256 x 256 PNGs in Art/UI/Icons/T_Icon_Stat_<Stat>.png. Stats: Damage, Fire rate, Reload speed, Crit chance, Crit damage, Piercing, Max health, Move speed, Pickup radius, XP gain, Mining speed, Luck (no Magazine size, not offered in M1).
- done_when: Candidates generated and self-reviewed; owner picked one per stat; 12 transparent PNGs exist; retained images, prompts and logs committed, others untracked.
- editor_state: any
- token_ceiling: open
- model: sonnet
- tokens: input 58, cache write 329043, cache read 1635749, output 4806
- runs: 4
- milestone: M2
- result: 12 stat icons, owner picks (damage-2, fire-rate-1, reload-speed-2, crit-chance-2, crit-damage-2, piercing-2, max-health-1, move-speed-1, pickup-radius-2, mining-speed-1, luck-2; XP gain redone twice, owner picked the stylised XP letters with an up arrow c-1). Keyed with Tools/art/ui_icon_prepare.js to Art/UI/Icons/T_Icon_Stat_<Stat>.png, 256 x 256. About 312 credits, 144 lost to parallel Kie runs overwriting each other (lesson in art-3d-lessons). Commits fdb9923, 84bbb2a.

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
