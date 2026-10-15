---
id: task-168-06-card-layout-fix
description: "Fix the card layout seen in game: backgrounds behind the frame, content inside the frame opening, thinner frame, larger cards and icons."
parent: task-168-upgrade-card-design
kind: task
status: done
---
# Upgrade cards: layout fix after the owner test

- goal: Make the stat and new-weapon cards of WBP_UpgradeChoice render as the validated mockup: frame proportions, content inside the opening, larger cards.
- agent: bp
- depends_on: task-168-05-upgrade-choice-layout, task-169-04-weapon-card-layout
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md); owner screenshot described in the delegation; mockup Art/Source/card-mockup/epic-card-spec.png
- expected_output: Owner feedback 2026-10-14 fixed in WBP_UpgradeChoice for both card kinds: (1) one dark panel (#15171c) BEHIND the whole card, under the frame layers; the image area (#1c1f26) inside the frame opening, never drawn over the frame; (2) all content (image area, effect, target, explanation, lore) inside the frame opening with about 10 px padding: opening safe rect on the 1086 x 1448 textures = left 141, top 155 (weapon frame 185, header plate), right 142, bottom 163 px, scaled by the drawn frame scale; (3) frame drawn at about 0.25 of the texture (side band about 28 px, corners about 55 px at 1080p), verified by reading back the real drawn size (Box margins are fractions of the brush ImageSize: ImageSize must be the texture size times the scale); (4) cards about 440 x 720 at 1080p, row starting higher and ending lower (may cover the weapon slots during the choice), title on top, reroll centred below; (5) stat icon alone (All weapons or player stat) 125 x 125 instead of 100; (6) the small dark square at the top left of each card removed; lore still anchored at the bottom of the opening; widget names kept; doc updated.
- done_when: Compiles with warnings as errors, saved, tree and brush values read back (drawn frame band and opening sizes reported in px), doc updated, committed; existing integration tests not run here (editor open). Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 36, cache write 51662, cache read 624360, output 552
- runs: 1
- milestone: M2
- result: Causes: frame and panel overlay slots Left/Top aligned (not stretched), CardPanel an empty NoImage Image (transparent centre; the 32 x 32 default size is the likely dark square), brush ImageSize 380 x 507 (79 px margins). Fixed: CardPanel RoundedBox #15171c Fill under the frames; four frame brushes ImageSize 271.5 x 362 (scale 0.25), Fill slots; CardSize 452 x 732 (440 x 720 stack); CardBox padding 45/49/46/51 (opening inset x 0.25 + 10), WeaponBody 45/56/46/115; ImageArea 348 x 320, WeaponBox 190 x 80, stat icon 125 x 125; lore bottom-anchored. Computed: side band about 35 px, top 39 (weapon 46), bottom 41, opening about 369 x 640. Widget names kept. CompileWidgetBlueprint true, saved, doc updated. Not seen on screen. Commit 73c31e5.

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
