---
id: _paths
description: "Generated id -> path map for resolving a wikilink you already hold to its file. Regenerate with: node scripts/lint.js --write. Never edit by hand."
parent: root
kind: note
---
# Wikilink resolution map (generated)

One line per node: `id  path`. Use it to resolve a wikilink whose target is not in the
directory you are reading (cross-branch jump). This is resolution, not search: you
must already hold the id from a `## Relations` gloss. Do not read it to find a topic.

```
agent-bp  reference/agents/agent-bp.md
agent-cpp  reference/agents/agent-cpp.md
agent-level-design  reference/agents/agent-level-design.md
agent-orchestrator  reference/agents/agent-orchestrator.md
agents-index  reference/agents/agents-index.md
arena-grid  reference/gdd/terrain/arena-grid.md
art-audio-index  reference/gdd/art-audio/art-audio-index.md
asset-pipeline  reference/tech/pipelines/asset-pipeline.md
audio-direction  reference/gdd/art-audio/audio-direction.md
barrel  reference/gdd/terrain/barrel.md
blueprints-index  reference/tech/blueprints/blueprints-index.md
bp-arena-grid  reference/tech/blueprints/bp-arena-grid.md
build-toolchain  reference/tech/pipelines/build-toolchain.md
burst-rifle  reference/gdd/weapons/burst-rifle.md
color-code  reference/gdd/art-audio/color-code.md
conventions-index  reference/tech/conventions/conventions-index.md
course-index  reference/course/course-index.md
cpp-bp-split  reference/tech/conventions/cpp-bp-split.md
cpp-standard  reference/tech/conventions/cpp-standard.md
damage-numbers  reference/gdd/weapons/damage-numbers.md
destruction-rules  reference/gdd/terrain/destruction-rules.md
dialogue-tone  reference/gdd/art-audio/dialogue-tone.md
difficulty  reference/gdd/run/difficulty.md
drone  reference/gdd/player/drone.md
editor-workflow  reference/tech/conventions/editor-workflow.md
enemies-index  reference/gdd/enemies/enemies-index.md
enemy-chaser  reference/gdd/enemies/enemy-chaser.md
enemy-death  reference/gdd/enemies/enemy-death.md
enemy-heavy  reference/gdd/enemies/enemy-heavy.md
enemy-kamikaze  reference/gdd/enemies/enemy-kamikaze.md
enemy-shooter  reference/gdd/enemies/enemy-shooter.md
explosions  reference/gdd/terrain/explosions.md
extraction  reference/gdd/run/extraction.md
gas-api-notes  reference/tech/systems/gas-api-notes.md
gas-engine-facts  reference/tech/systems/gas-engine-facts.md
gas-scope  reference/production/gas-scope.md
gdd-index  reference/gdd/gdd-index.md
heal-ore  reference/gdd/terrain/heal-ore.md
layout-generator-recipe  reference/tech/pipelines/layout-generator-recipe.md
level-flow  reference/gdd/run/level-flow.md
level-layout-schema  reference/tech/pipelines/level-layout-schema.md
lore  reference/gdd/lore.md
milestones  reference/production/milestones.md
miner  reference/gdd/player/miner.md
naming  reference/tech/conventions/naming.md
objective-bonus-chest  reference/gdd/run/objective-bonus-chest.md
objective-console  reference/gdd/run/objective-console.md
objective-variants  reference/gdd/run/objective-variants.md
orchestration-protocol  reference/agents/orchestration-protocol.md
ore-blocks  reference/gdd/terrain/ore-blocks.md
ore-economy  reference/gdd/run/ore-economy.md
perf-risks  reference/tech/performance/perf-risks.md
performance-index  reference/tech/performance/performance-index.md
pipelines-index  reference/tech/pipelines/pipelines-index.md
player-index  reference/gdd/player/player-index.md
player-stats  reference/gdd/player/player-stats.md
production-index  reference/production/production-index.md
rarities  reference/gdd/weapons/rarities.md
readability-rules  reference/gdd/art-audio/readability-rules.md
root  root.md
run-index  reference/gdd/run/run-index.md
run-log-schema  reference/tech/pipelines/run-log-schema.md
run-structure  reference/gdd/run/run-structure.md
shotgun  reference/gdd/weapons/shotgun.md
stress-test-m1  reference/tech/performance/stress-test-m1.md
sys-auto-fire  reference/tech/systems/sys-auto-fire.md
sys-damage  reference/tech/systems/sys-damage.md
sys-difficulty  reference/tech/systems/sys-difficulty.md
sys-enemy-ai  reference/tech/systems/sys-enemy-ai.md
sys-explosions  reference/tech/systems/sys-explosions.md
sys-gas-core  reference/tech/systems/sys-gas-core.md
sys-grid  reference/tech/systems/sys-grid.md
sys-player  reference/tech/systems/sys-player.md
sys-projectiles  reference/tech/systems/sys-projectiles.md
sys-status-effects  reference/tech/systems/sys-status-effects.md
sys-upgrades  reference/tech/systems/sys-upgrades.md
sys-weapons  reference/tech/systems/sys-weapons.md
systems-index  reference/tech/systems/systems-index.md
task-000-enable-gas  reference/tasks/task-000-enable-gas.md
task-001-01-validator-void  reference/tasks/task-001-arena-m1/task-001-01-validator-void.md
task-001-02-layout-arena-m1  reference/tasks/task-001-arena-m1/task-001-02-layout-arena-m1.md
task-001-03-arena-grid-cpp  reference/tasks/task-001-arena-m1/task-001-03-arena-grid-cpp.md
task-001-04-greybox-materials  reference/tasks/task-001-arena-m1/task-001-04-greybox-materials.md
task-001-05-bp-arena-grid  reference/tasks/task-001-arena-m1/task-001-05-bp-arena-grid.md
task-001-06-map-arena-m1  reference/tasks/task-001-arena-m1/task-001-06-map-arena-m1.md
task-001-arena-m1  reference/tasks/task-001-arena-m1.md
task-002-01-twinstick-defaults  reference/tasks/task-002-cleanup-twinstick/task-002-01-twinstick-defaults.md
task-002-02-delete-template-content  reference/tasks/task-002-cleanup-twinstick/task-002-02-delete-template-content.md
task-002-03-layout-rock-60  reference/tasks/task-002-cleanup-twinstick/task-002-03-layout-rock-60.md
task-002-04-delete-template-cpp  reference/tasks/task-002-cleanup-twinstick/task-002-04-delete-template-cpp.md
task-002-05-verify-pie  reference/tasks/task-002-cleanup-twinstick/task-002-05-verify-pie.md
task-002-cleanup-twinstick  reference/tasks/task-002-cleanup-twinstick.md
task-003-01-layout-80-paths  reference/tasks/task-003-arena-80-paths/task-003-01-layout-80-paths.md
task-003-02-map-update  reference/tasks/task-003-arena-80-paths/task-003-02-map-update.md
task-003-03-project-name  reference/tasks/task-003-arena-80-paths/task-003-03-project-name.md
task-003-arena-80-paths  reference/tasks/task-003-arena-80-paths.md
task-004-01-enable-gas  reference/tasks/task-004-miner-gas/task-004-01-enable-gas.md
task-004-02-gas-core  reference/tasks/task-004-miner-gas/task-004-02-gas-core.md
task-004-03-miner-classes  reference/tasks/task-004-miner-gas/task-004-03-miner-classes.md
task-004-04-bp-miner  reference/tasks/task-004-miner-gas/task-004-04-bp-miner.md
task-004-miner-gas  reference/tasks/task-004-miner-gas.md
task-005-01-drone-cpp  reference/tasks/task-005-drone-orbit/task-005-01-drone-cpp.md
task-005-02-bp-drone  reference/tasks/task-005-drone-orbit/task-005-02-bp-drone.md
task-005-drone-orbit  reference/tasks/task-005-drone-orbit.md
tasks-index  reference/tasks/tasks-index.md
tech-index  reference/tech/tech-index.md
terrain-index  reference/gdd/terrain/terrain-index.md
turret  reference/gdd/weapons/turret.md
unreal-mcp  reference/tech/pipelines/unreal-mcp.md
upgrades  reference/gdd/weapons/upgrades.md
visual-style  reference/gdd/art-audio/visual-style.md
weapons-index  reference/gdd/weapons/weapons-index.md
wishlist  reference/production/wishlist.md
xp-level-up  reference/gdd/run/xp-level-up.md
```

## Relations
- [[_navigation]] — the protocol that says when to open this map.
- [[root]] — the map this file serves.
