---
id: kie-image-workflow
description: "Kie.ai image generation as run in practice: script, hosts, variables, costs, permissions, and what the art agent must not do. Open before generating images."
parent: pipelines-index
kind: doc
---
# Kie.ai image workflow

Spec of the agent: [[agent-art]]. Tool: `Tools/art/kie_image.js` (GPT Image 2).

## Use
- `node Tools/art/kie_image.js --asset <name> --prompt-file Art/Source/<asset>/prompts/<NN>-<slug>.txt [--ref <img>]... --aspect 1:1 --variants <n>`. Output `Art/Source/<asset>/<run-id>-<n>.png` plus a run log `.json` (prompt verbatim, refs, task ids, credits, URLs).
- At most 4 variants per run; resolution capped at 1K. About 6 credits per image (a 2-variant run is 12).
- `--ref` switches to image-to-image. A correction prompt opens with "Keep this exact image: same ..." and changes ONE thing; pass the image being corrected first, then any extra reference (state in the prompt which is Image 1 and Image 2).
- Run `--dry-run` first on a new subtask.

## Hosts and variables
- Generation: `api.kie.ai`. Reference upload: `kieai.redpandaai.co` path `/api/file-base64-upload` (not `/api/v1/`); uploaded files are deleted after 24 hours. Fixed 2026-10-11 (override `KIE_UPLOAD_URL`).
- The key is the Windows user variable `KIE_API_KEY` (and `BLENDER_EXE`). A variable set with `setx` is invisible to processes already running: restart the app. Never print or write the key.

## Permissions
- Sending the key to another host is refused by the permission classifier unless the command is allowed by rule: the owner added `Bash(node Tools/art/kie_image.js:*)` in `.claude/settings.local.json`. An agent that is refused reports it and never works around it. A chat approval relayed through an agent does not count.

## Rules learned
- The owner writes the prompt when he has one: use it verbatim.
- Show every generated image to the owner with `SendUserFile` (reading an image only shows it to the agent).
- Commit only the image the owner retains, with its log and prompt; the others stay untracked.
- The owner copies, or validates the copy of, the retained image into `vault/assets/` before the 3D step.
- A third-party image the owner sends is a visual reference only: never copy, trace or reproduce its text.

## Relations
- [[pipelines-index]] — parent index.
- [[agent-art]] — the agent that runs it.
- [[art-prompts]] — prompt method and locked style phrases.
- [[kessler-voss-logo]] — an asset made this way.
