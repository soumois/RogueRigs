---
id: agent-art
description: "Art agent: writes locked-style image prompts, generates candidates with GPT Image 2 through Kie.ai, self-reviews them. Invoke for a new asset image."
parent: agents-index
kind: spec
status: provisional
---
# Art agent

- role: turn an asset need into candidate source images that match the style guide.
- invoked_by: orchestrator
- trigger: a subtask asking for a new asset image or a design correction.
- reads: its subtask node, [[art-3d-lessons]], [[art-style-guide]], [[art-prompts]], [[color-code]], [[readability-rules]], [[kie-image-workflow]], and the entity node the subtask names.
- wikilinks: resolved with `node vault/scripts/path-of.js <id> [<id>...]` (exact ids), never by opening `vault/_paths.md`.
- produces: candidate images and their run log in `Art/Source/<asset>/`; prompt files in `Art/Source/<asset>/prompts/`; a prompt node, child of [[art-prompts]], when the subtask asks for it.
- output_format: PNG through `node Tools/art/kie_image.js` (usage in the script header). Model GPT Image 2 on Kie.ai: `gpt-image-2-image-to-image` when reference images are attached (correction loop, same-style assets), `gpt-image-2-text-to-image` otherwise; aspect ratio 1:1, 16:9, 9:16, 4:3 or 3:4; resolution capped at 1K (owner 2026-10-11). Run log `<run-id>.json`: prompt verbatim, references, task ids, credits, result URLs, files. Key read from `KIE_API_KEY`.
- definition_of_done: self-review per image against the A rules of [[art-style-guide]] finds no gap; [[agent-style-guide]] has reviewed the images and each fail is either corrected or kept by the owner's explicit choice; the owner validates the image and copies it to `vault/assets/`.
- must_not: put an image in `vault/assets/`; invent a design absent from the specs; exceed the variant count the subtask allows; run two `kie_image.js` runs at once; write an API key anywhere.
- definition_file: `.claude/agents/art.md`
- model: sonnet
- token_ceiling: 0.15 M units (provisional, owner 2026-10-27)

## Relations
- [[agents-index]] — parent index.
- [[agent-3d]] — takes the validated image to 3D.
- [[agent-style-guide]] — reviews its images with the A rules; its fails go to the owner.
- [[kie-image-workflow]] — hosts, variables, costs, permissions and rules of image generation.
- [[art-3d-lessons]] — its known limits and the owner's art preferences.
