# Assignment 7 — Complete AI Dev Pipeline

**ELVTR · Multi-Agent AI for Game Development** · Soumois Pierre-Emmanuel

> **Status: DRAFT.** Due 3 November 2026, 11:59 PM GMT. Items marked `[TO FILL]` need the owner. Every number comes from `vault/reference/production/cost-log.md` and `vault/scripts/usage-snapshot.json`.

## Student & game

- **Student:** Soumois Pierre-Emmanuel
- **Game:** *Rogue Rigs*
- **Concept:** top-down 3D roguelite auto-shooter in Unreal Engine 5.8 (C++). An independent miner steals ore from a corporate asteroid claim. The player aims by hand, weapons fire automatically, and the destructible rock arena is the main weapon. A run is 4 levels; upgrades carry over. No AI runs inside the game: agents are development tools only.

## Deliverable 1 — Playable link

- **Playable game link:** `[TO FILL]`
- Unreal Engine cannot run in a browser. Realistic option: Windows build zipped and hosted on itch.io (download, unzip, run `RogueRigs.exe`, no installer). The last staged build in the repo dates from 2026-10-10 and is outdated.

## Deliverable 2 — Pipeline source code & engine integration

- **Pipeline repository:** https://github.com/soumois/RogueRigs `[TO FILL: confirm the repo is public]`
- **Pipeline run video:** `[TO FILL]`
- **Target engine:** Unreal Engine 5.8 (C++ first, thin Blueprint layer)
- **Automated flow:**
  1. The owner types `/task <request>`. The Orchestrator checks it against the GDD, splits it into subtask nodes (`vault/reference/tasks/`), and the Executor runs them in order.
  2. **Level-Design agent** writes `Content/RogueRigs/Data/Levels/*.json`; `Tools/validate_layout.js` rejects invalid layouts; the C++ `ARRArenaGrid` loads the file at level start.
  3. **Text agent** writes `Content/RogueRigs/Data/Text/*.json` (barks, alerts, upgrade lines); the **Style-guide agent** scores and the refiner rewrites (A6 loop); `Tools/content/validate_text.js` gates the files; the C++ `RRTextContentSubsystem` loads them and shows them in game.
  4. **C++ agent** writes `Source/RogueRigs/`, built from the command line with automation tests (185 headless tests).
  5. **BP agent** creates Blueprints, materials and maps through the Unreal MCP server; **Art** and **3D** agents turn prompts into images and meshes (Meshy/Tripo, Blender MCP) imported through the same MCP.
  6. Each subtask ends with a git commit and a cost row.

---

## Deliverable 3 — Pipeline audit & cost analysis (1 page)

### Pipeline production & functionality

**Produced, present in the build:**
- 5 arena layouts (JSON: `arena_m1`, `m1_fine`, `l2`, `l3`, `l4`) validated by script.
- 69 player-facing text lines (34 barks, 15 alerts, 13 upgrade lines, 5 screen texts, 2 weapon texts) passed through the style-guide loop.
- C++ gameplay: grid with destruction, flow field, GAS weapons, 5 robot types, wave director, XP/levels/rarities, shuttle extraction, shop.
- Blueprints, UMG widgets, Niagara effects; meshes for 8+ robots and props through the art → 3D chain.

**Manual steps left:**
1. The owner validates each generated image before it goes to 3D.
2. Editor sequencing: C++ builds need the editor closed, Blueprint work needs it open.
3. Visual checks and playtests: the owner reviews screenshots and videos.
4. Packaging, cooking and uploading the build: no agent does it yet.

**To reach 100% automation:** a packaging agent running `RunUAT BuildCookRun` plus an itch.io `butler push` step; a headless render check with an automated screenshot diff instead of owner review; an image-quality evaluator (the A6 loop applied to images) to replace the owner's image approval.

### Architectural reflection

- **Decision to change:** one long-lived Orchestrator session for planning and supervising everything. The first main session ran 335 calls, 22 resumes, peaked at 779k tokens of context and re-read 137 M cache tokens.
- **Alternative:** a stateless Executor per global task, fed only by the task node and resolved paths, with state kept in vault files and a short handover note. Context stays below ~100k per run. The project started moving this way on 2026-10-26 (Executor and `token_ceiling` per agent).

### Cost analysis

- **Total actual run cost: ≈ $1,263** (API list prices, tasks 013 → 350, 2026-10-03 → 2026-10-28). Units from the cost log × input price (Opus 5.5 $4/M, Sonnet 5.5 $2/M; cache write ×1.25, cache read ×0.1, output ×5, as in `cost-rates.json`; prices from [Anthropic's model page](https://platform.claude.com/docs/en/models/opus-5)).

| Scope | Units (M) | USD |
|---|---|---|
| Opus subtasks (mostly C++ agent) | 181.98 | 727.92 |
| Mixed-model subtasks (priced as Opus, upper bound) | 5.91 | 23.64 |
| Sonnet subtasks (BP, text, docs) | 128.24 | 256.48 |
| Orchestrator main sessions | 52.72 | 153.36 |
| Planning passes | 14.84 | 59.37 |
| Executors | 12.68 | 41.08 |
| Other | 0.49 | 0.97 |
| **Total** | **396.86** | **≈ 1,262.82** |

  Not included: tasks 000–012 (no subtask id), image/3D/music generation outside Claude `[TO FILL: Meshy, image model, Suno/kie.ai amounts]`. Cache read is 50% of subtask units, cache write 29%, output 21%.
- **Most expensive step:** the C++ agent on Opus, about $728 (58% of the total). Most expensive single subtask: `task-284-01-parts-bake-heavy` (Sonnet, 3 runs, 5.57 M units ≈ $11.14).
- **Sustainability:** `[TO CONFIRM]` At list prices, ≈ $1.26k for 25 days is not sustainable for an unfunded solo developer. On a flat-rate plan or with the per-agent `token_ceiling` limits (0.15–1.2 M units) it is sustainable; the cost is dominated by re-read context, which smaller subtasks reduce.

### Mid-project cost-reduction change

- **Approach — before:** Blueprint subtasks bundled many assets (`task-014-04`, `016-03`, `022-02`, `024-05`). **After (owner rule 2026-10-14):** one main asset per BP subtask.
- **Cost — before:** 4 subtasks, average 2.46 M units (≈ $4.93) and 4.5 runs each. **After:** 57 subtasks (task ≥ 203), average 0.28 M units (≈ $0.57) and 1.02 runs each.
- Caveat: compares per-subtask averages; the bundled subtasks covered more than one asset each.
