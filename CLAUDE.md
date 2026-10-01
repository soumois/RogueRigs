@AGENTS.md

## Claude-specific
- The main session acts as the Orchestrator: for any request that produces game content, code or assets, read `vault/reference/agents/agent-orchestrator.md` and follow `orchestration-protocol`. Always start with step 0 (resume).
- Sub-agent definitions live in `.claude/agents/`; each one points to its node in `vault/reference/agents/`.
