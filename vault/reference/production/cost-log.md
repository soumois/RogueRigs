---
id: cost-log
description: "Input, cache and output tokens, model and runs per subtask from task 013, with totals; read to estimate a future project cost."
parent: production-index
kind: note
---
# Cost log

- Source: `node vault/scripts/agent-usage.js` (add `--json`, `--from <n>`). It reads this machine's Claude Code transcripts, matches each agent to its subtask by the id at the start of its description, and counts every API call once. Delegations must therefore start their description with the subtask id.
- Columns are billed separately and at different prices: input (uncached), cache write (context written to the prompt cache), cache read (context re-read from the cache on each turn, the cheapest but by far the largest), output (the most expensive per token).
- Runs: agent launches including resumes (a resume continues the same transcript).
- Not covered: tasks 000 to 012 (their agents carry no subtask id in the description).
- Each finished subtask adds a row ([[orchestration-protocol]] step 5).

## Per subtask
| Subtask | Model | Runs | Input | Cache write | Cache read | Output |
|---|---|---|---|---|---|---|
| [[task-013-01-baseline]] | opus | 1 | 0k | 108k | 3940k | 46k |
| [[task-013-02-final]] | opus | 1 | 0k | 117k | 2652k | 50k |
| [[task-014-01-spawn-packs]] | opus | 1 | 0k | 90k | 1684k | 40k |
| [[task-014-02-upgrade-system]] | opus | 1 | 0k | 154k | 3647k | 65k |
| [[task-014-03-levelup-flow]] | opus | 1 | 0k | 143k | 5033k | 59k |
| [[task-014-04-bp-upgrade-ui]] | sonnet | 4 | 0k | 242k | 19229k | 81k |
| [[task-014-05-tests-integration]] | opus | 2 | 0k | 172k | 6700k | 65k |
| [[task-015-01-shotgun-cpp]] | opus | 2 | 0k | 196k | 8481k | 72k |
| [[task-015-02-bp-shotgun]] | sonnet | 2 | 0k | 121k | 4445k | 31k |
| [[task-016-01-alert-console]] | opus | 2 | 0k | 251k | 11240k | 116k |
| [[task-016-02-shuttle]] | opus | 1 | 0k | 119k | 2610k | 48k |
| [[task-016-03-bp-objectives]] | sonnet | 4 | 0k | 202k | 16009k | 69k |
| [[task-016-04-tests-integration]] | opus | 2 | 0k | 170k | 2291k | 44k |
| [[task-017-01-radius]] | opus | 1 | 0k | 51k | 515k | 8k |
| [[task-018-01-trace]] | opus | 1 | 0k | 84k | 2444k | 29k |
| [[task-018-02-crowd-collision]] | opus | 1 | 0k | 117k | 3839k | 41k |
| [[task-018-03-light-movement]] | opus | 1 | 0k | 95k | 2968k | 37k |
| [[task-018-04-hitch]] | opus | 1 | 0k | 70k | 1694k | 20k |
| [[task-018-05-final-stress]] | opus | 1 | 0k | 162k | 538k | 17k |
| [[task-019-01-asset-manager]] | opus | 1 | 0k | 36k | 390k | 8k |
| [[task-020-01-review]] | opus | 1 | 0k | 188k | 7197k | 38k |
| [[task-020-02-refactor]] | opus | 2 | 0k | 191k | 7042k | 69k |
| [[task-020-03-refactor-cleanup]] | opus | 1 | 0k | 117k | 3705k | 40k |
| [[task-020-04-bp-check]] | sonnet | 1 | 0k | 76k | 1578k | 9k |
| [[task-021-01-shooter-cpp]] | opus | 1 | 0k | 200k | 6584k | 87k |
| [[task-021-02-bp-shooter]] | sonnet | 2 | 0k | 138k | 6637k | 39k |
| [[task-021-03-tests-integration]] | opus | 1 | 0k | 106k | 1812k | 25k |
| [[task-022-01-heavy-cpp]] | opus | 1 | 0k | 142k | 3564k | 44k |
| [[task-022-02-bp-heavy]] | sonnet | 4 | 0k | 349k | 9269k | 59k |
| [[task-022-03-tests-integration]] | opus | 2 | 0k | 127k | 2801k | 30k |
| [[task-023-01-cell-stress]] | opus | 2 | 0k | 458k | 7854k | 67k |
| [[task-024-01-rifle-pierce]] | opus | 1 | 0k | 170k | 4373k | 56k |
| [[task-024-02-shotgun-pellets]] | opus | 1 | 0k | 184k | 5599k | 79k |
| [[task-024-03-turret]] | opus | 2 | 0k | 206k | 9284k | 87k |
| [[task-024-04-weapon-cards]] | opus | 1 | 0k | 152k | 4094k | 68k |
| [[task-024-05-bp-weapons]] | sonnet | 6 | 0k | 245k | 26014k | 87k |
| [[task-024-06-tests-integration]] | opus | 2 | 0k | 214k | 5029k | 61k |
| [[task-025-01-numbers-pipeline]] | opus | 1 | 0k | 150k | 3989k | 55k |
| [[task-025-02-bp-numbers]] | sonnet | 2 | 0k | 154k | 7174k | 52k |
| [[task-025-03-numbers-perf]] | opus | 2 | 0k | 175k | 7657k | 55k |
| [[task-025-04-per-digit-contract]] | opus | 1 | 0k | 145k | 4931k | 58k |
| [[task-025-05-bp-numbers-rebuild]] | sonnet | 3 | 0k | 163k | 9492k | 65k |
| [[task-025-06-numbers-visual-check]] | opus | 1 | 0k | 89k | 1720k | 19k |
| [[task-029-01-grid-render]] | opus | 2 | 0k | 598k | 10948k | 75k |
| [[task-029-02-flow-field-incremental]] | opus | 1 | 0k | 176k | 6095k | 87k |
| [[task-029-03-layout-67]] | sonnet | 1 | 0k | 55k | 461k | 1k |
| [[task-029-04-switch-67]] | opus | 2 | 0k | 339k | 11663k | 44k |
| [[task-029-05-arc-mining]] | opus | 1 | 0k | 160k | 6134k | 80k |
| [[task-029-06-bp-fine-grid]] | sonnet | 1 | 0k | 72k | 1938k | 18k |
| [[task-029-08-laser-fan]] | opus | 1 | 0k | 58k | 716k | 16k |
| [[task-029-09-bp-fan-holes]] | sonnet | 2 | 0k | 79k | 2354k | 29k |
| [[task-029-07-final-check]] | opus | 2 | 0k | 178k | 8102k | 54k |
| [[task-026-00-landing-clearance]] | opus | 1 | 0k | 83k | 2512k | 32k |
| [[task-026-01-wave-director]] | opus | 1 | 0k | 173k | 5774k | 99k |
| [[task-026-02-feedback-events]] | opus | 1 | 0k | 131k | 4276k | 50k |
| [[task-026-03-bp-pressure]] | sonnet | 2 | 0k | 121k | 3736k | 41k |
| [[task-026-04-tests-integration]] | opus | 2 | 0k | 396k | 9265k | 83k |
| [[task-027-01-pickup-motion]] | opus | 2 | 0k | 205k | 9515k | 92k |
| [[task-027-02-bp-juice]] | sonnet | 2 | 0k | 129k | 5708k | 46k |
| [[task-027-03-tests-integration]] | opus | 2 | 0k | 226k | 11364k | 76k |
| [[task-028-01-heavy-shield-values]] | opus | 2 | 0k | 172k | 6777k | 58k |
| [[task-028-02-bp-shield-heavy]] | sonnet | 2 | 0k | 132k | 6469k | 46k |
| [[task-028-03-xp-pacing]] | opus | 2 | 0k | 426k | 8439k | 62k |
| [[task-030-01-director-speeds]] | opus | 1 | 0k | 171k | 4580k | 53k |
| [[task-030-02-pacing-retune]] | opus | 2 | 0k | 501k | 6689k | 51k |
| [[task-031-01-remove-dummy]] | opus | 2 | 0k | 155k | 5632k | 38k |
| [[task-031-02-spawn-speed-shooter]] | opus | 3 | 0k | 399k | 10460k | 89k |
| [[task-032-01-cpp-fixes]] | opus | 2 | 0k | 743k | 22786k | 139k |
| [[task-032-02-bp-fixes]] | sonnet | 2 | 0k | 109k | 3779k | 32k |
| [[task-031-03-pacing-retune]] | opus | 2 | 0k | 790k | 7962k | 72k |
| [[task-033-01-update-tests]] | opus | 2 | 0k | 77k | 1795k | 22k |
| [[task-034-01-cpp-fixes]] | opus | 3 | 0k | 211k | 10174k | 68k |
| [[task-034-03-docs]] | opus | 1 | 0k | 91k | 1414k | 16k |
| [[task-034-04-director-mix]] | opus | 2 | 0k | 95k | 2042k | 27k |
| [[task-035-01-heal-value]] | opus | 1 | 0k | 40k | 487k | 6k |
| [[task-036-01-relocate]] | opus | 1 | 0k | 147k | 4931k | 49k |
| [[task-037-01-hud-ammo-data]] | opus | 1 | 0k | 56k | 592k | 11k |
| [[task-037-02-bp-ammo-hud]] | sonnet | 2 | 0k | 99k | 2916k | 34k |
| [[task-038-01-shooter-detour]] | opus | 1 | 0k | 109k | 3137k | 30k |
| [[task-039-01-heavy-radius]] | opus | 1 | 0k | 26k | 294k | 7k |
| [[task-041-01-front-detour]] | opus | 2 | 0k | 34k | 589k | 8k |
| [[task-042-01-hack-time]] | opus | 1 | 0k | 22k | 239k | 5k |
| [[task-043-01-vein-shake]] | opus | 1 | 0k | 34k | 542k | 10k |
| [[task-040-01-locator-arrow]] | sonnet | 2 | 0k | 151k | 5575k | 40k |
| [[task-044-01-fewer-kamikazes]] | opus | 1 | 0k | 47k | 977k | 14k |
| [[task-045-01-easier-first-level]] | opus | 1 | 0k | 141k | 1122k | 15k |
| [[task-045-02-bp-heavy-cap]] | sonnet | 1 | 0k | 62k | 941k | 8k |
| [[task-048-01-last-level-profile]] | opus | 1 | 0k | 33k | 235k | 5k |
| [[task-046-01-level1-pressure]] | opus | 1 | 0k | 229k | 1032k | 28k |
| [[task-047-01-heavy-speed]] | opus | 1 | 0k | 40k | 191k | 4k |
| [[task-049-01-shuttle-distance]] | opus | 1 | 0k | 51k | 760k | 13k |
| [[task-050-01-flaky-stacking-test]] | opus | 1 | 0k | 54k | 987k | 14k |
| [[task-051-01-turret-spacing]] | opus | 1 | 0k | 50k | 613k | 10k |
| [[task-052-01-heavy-radius-2]] | opus | 1 | 0k | 34k | 434k | 7k |
| [[task-055-01-first-hit-vignette]] | opus | 2 | 0k | 300k | 3962k | 39k |
| [[task-056-01-bonus-chest]] | opus | 1 | 0k | 132k | 4869k | 42k |
| [[task-053-01-weapon-info-centred]] | sonnet | 2 | 0k | 56k | 1184k | 16k |
| [[task-054-01-niagara-trail-sparkle]] | sonnet | 2 | 0k | 78k | 2163k | 23k |
| [[task-056-02-bp-bonus-chest]] | sonnet | 2 | 0k | 82k | 3135k | 28k |
| [[task-054-02-juice-test]] | opus | 2 | 0k | 112k | 1044k | 19k |
| [[task-055-02-bp-flash-hold]] | sonnet | 1 | 0k | 48k | 636k | 10k |
| [[task-057-01-flaky-xp-pickup-test]] | opus | 1 | 0k | 77k | 578k | 10k |
| [[task-059-01-damage-numbers-no-merge]] | opus | 1 | 0k | 213k | 2835k | 38k |
| [[task-060-01-deep-mining]] | opus | 1 | 0k | 153k | 1244k | 19k |
| [[task-061-01-minimap]] | opus | 1 | 0k | 135k | 1217k | 29k |
| [[task-058-01-xp-trail-visible]] | sonnet | 1 | 0k | 47k | 728k | 11k |
| [[task-061-02-bp-minimap]] | sonnet | 2 | 0k | 100k | 3758k | 41k |
| [[task-062-01-alert-bar-x2]] | sonnet | 1 | 0k | 49k | 843k | 12k |
| [[task-063-01-perf-session]] | opus | 1 | 0k | 232k | 1527k | 20k |
| [[task-064-01-minimap-circle-reveal]] | opus | 1 | 0k | 76k | 460k | 9k |
| [[task-065-01-diagonal-dig]] | opus | 1 | 0k | 82k | 499k | 14k |
| [[task-062-02-pressure-test]] | opus | 1 | 0k | 32k | 172k | 4k |
| [[task-066-01-async-flow-field]] | opus | 1 | 0k | 273k | 3725k | 49k |
| [[task-066-02-perf-check]] | opus | 1 | 0k | 98k | 964k | 14k |
| [[task-067-01-real-game-stress]] | opus | 1 | 0k | 183k | 5745k | 52k |
| [[task-068-01-shipping-insights]] | opus | 2 | 0k | 146k | 5846k | 48k |
| [[task-072-02-film-pool]] | opus | 1 | 0k | 33k | 67k | 6k |
| [[task-072-01-film-pool]] | sonnet | 1 | 0k | 17k | 86k | 6k |
| [[task-073-01-text-schema]] | opus | 1 | 0k | 55k | 263k | 18k |
| [[task-074-01-text-validator]] | opus | 1 | 0k | 53k | 473k | 27k |
| [[task-075-01-baseline-run]] | sonnet | 1 | 0k | 21k | 46k | 4k |
| [[task-078-02-alert-texts]] | opus | 2 | 0k | 33k | 74k | 5k |
| [[task-076-02-upgrade-texts]] | opus | 3 | 0k | 68k | 150k | 9k |
| [[task-077-02-barks]] | opus | 3 | 0k | 78k | 242k | 14k |
| [[task-079-01-text-loader]] | opus | 2 | 0k | 295k | 4536k | 59k |
| [[task-081-01-a4-readme]] | sonnet | 1 | 0k | 178k | 3297k | 45k |
| [[task-082-02-shuttle-lines]] | opus | 3 | 0k | 73k | 187k | 11k |
| [[task-083-02-voice-tweak]] | opus | 3 | 0k | 69k | 153k | 11k |
| [[task-086-01-a4-readme-update]] | sonnet | 1 | 0k | 133k | 2425k | 36k |
| [[task-070-01-ore-in-rock]] | opus | 1 | 0k | 43k | 453k | 13k |
| [[task-071-01-robot-light-movement]] | opus | 2 | 0k | 580k | 9409k | 74k |
| [[task-087-01-card-desc-test]] | opus | 1 | 0k | 36k | 485k | 7k |
| [[task-088-01-heal-veins-2x2]] | sonnet | 1 | 0k | 28k | 155k | 6k |
| [[task-088-02-heal-veins-2x2]] | opus | 2 | 0k | 227k | 3937k | 39k |
| [[task-089-01-heal-not-at-full]] | opus | 1 | 0k | 160k | 1518k | 22k |
| [[task-090-01-actor-pool]] | opus | 1 | 0k | 101k | 2076k | 54k |
| [[task-091-01-robot-pool]] | opus | 3 | 0k | 318k | 22362k | 109k |
| [[task-092-01-xp-pool]] | opus | 2 | 0k | 170k | 6540k | 64k |

## Totals
| Scope | Input | Cache write | Cache read | Output |
|---|---|---|---|---|
| [[task-013-stress-test]] | 0k | 0.22 M | 6.59 M | 96k |
| [[task-014-upgrades-levelup]] | 1k | 0.80 M | 36.29 M | 310k |
| [[task-015-shotgun]] | 0k | 0.32 M | 12.93 M | 104k |
| [[task-016-console-extraction]] | 1k | 0.74 M | 32.15 M | 277k |
| [[task-017-kamikaze-radius]] | 0k | 0.05 M | 0.52 M | 8k |
| [[task-018-robot-perf]] | 0k | 0.53 M | 11.48 M | 144k |
| model opus | 1k | 2.10 M | 60.27 M | 757k |
| model sonnet | 1k | 0.56 M | 39.68 M | 181k |
| **All agents, tasks 013 to 018** | **2k | 2.66 M | 99.96 M | 938k** |
| Orchestrator, all main sessions of this project (also covers tasks 000 to 012 and non-task chat) | 1k | 4.47 M | 150.89 M | 438k |

## Lessons for estimates
- Per C++ subtask: output 20k to 115k, cache write 50k to 250k, cache read 0.5 M to 11 M.
- Blueprint subtasks through Unreal MCP needed 2 to 4 runs and re-read the most context (cache read 4 M to 19 M): budget them at least like C++ subtasks.
- The Orchestrator's own context, re-read on every turn of a long session, costs as much cache read as all the agents together: keep sessions shorter or hand over between global tasks.

## Relations
- [[production-index]] — parent index.
- [[orchestration-protocol]] — step 5 records each subtask here.
- [[tasks-index]] — the tasks measured.
