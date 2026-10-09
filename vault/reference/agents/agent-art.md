---
id: agent-art
description: "Art agent: writes image prompts in the locked style, generates candidates through the OpenAI image API and self-reviews them. Invoke for a new asset image."
parent: agents-index
kind: spec
status: provisional
---
# Art agent

- role: turn an asset need into candidate source images that match the style guide.
- invoked_by: orchestrator
- trigger: a subtask asking for a new asset image or a design correction (after Assignment #4).
- reads: its subtask node, [[art-style-guide]], [[art-prompts]], [[color-code]], [[readability-rules]], and the entity node the subtask names.
- produces: one prompt node per asset, child of [[art-prompts]]; candidate images in `Art/Source/<asset>/`.
- output_format: prompt node with the full prompt, the delivery format (sheet or T-pose) and the reference image to attach; images as PNG. API key read from `OPENAI_API_KEY`.
- definition_of_done: self-review against [[art-style-guide]] finds no gap; the owner validates the image, which is then copied to `vault/assets/`.
- must_not: put an image in `vault/assets/` without the owner; invent a design absent from the specs; write an API key anywhere.
- model: sonnet
- token_ceiling: open

## Relations
- [[agents-index]] — parent index.
- [[agent-3d]] — takes the validated image to 3D.
- [[agent-style-guide]] — will review its images with A rules.
