@AGENTS.md

## Claude-specific
- The owner starts crew work with `/task <request>` (`.claude/commands/task.md`). Without it, the main session still follows the same workflow for any request that produces game content, code or assets: it only relays. It launches the `planner` agent (`agent-planner`, protocol steps 0 to 4), relays the owner's questions and answers, then launches one fresh `executor` agent per confirmed global task (`agent-executor`, steps 5 and 6). It never plans or executes itself.
- Sub-agent definitions live in `.claude/agents/`; each one points to its node in `vault/reference/agents/`.
