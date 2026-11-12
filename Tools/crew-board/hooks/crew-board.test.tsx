import { expect, mock, test } from 'claude-code/testing'

const TASK = `---
id: task-318-preload
description: "Global task: preload."
parent: tasks-done-index
kind: task
status: done
---
# Preload the next level

- goal: Preload.
- milestone: M2
- result: Done.
`
const SUB = `---
id: task-318-01-preload
description: "Step."
parent: task-318-preload
kind: task
status: in-progress
---
# Step

- goal: Load in the background.
- agent: cpp
- token_ceiling: 60000
- model: opus
- tokens: input 88, cache write 133776, cache read 4262852, output 44106
- usage: turns 44, context start 11k peak 139k, resumes 0, 9 min, units 0.81 M
- runs: 2
- result: open
`
const DIR = 'C:/Game/vault/reference/tasks'
const PROJ = 'C:/Users/me/.claude/projects/C--Game'
// the folders are given, so the tests do not depend on where this repository sits
const OPTS = { options: { tasksDir: DIR, transcriptsDir: PROJ } }
const file = (name: string) => ({ name, kind: 'file' as const, size: 1, mtimeMs: 1, isLink: false })
const dir = (name: string) => ({ name, kind: 'dir' as const, size: 0, mtimeMs: 0, isLink: false })

test('the board lists a task read from the vault, on every surface', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  on('agent.list', () => ({ value: [] }))
  // the engine hands the plugin's folder option over with OS separators
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  on('fs.list', (_, e) => ({
    value: fwd(e.path) === DIR ? [file('task-318-preload.md'), dir('task-318-preload')] : [file('task-318-01-preload.md')],
  }))
  on('fs.read', (_, e) => ({ value: fwd(e.path).endsWith('/task-318-preload.md') ? TASK : SUB }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(200)

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ ...PANE, surface })
    const row = await ui.find({ type: 'Text', text: /Preload the next level/ })
    expect(row?.text).toMatch(/Preload/)
    await ui.unmount()
  }
})

const PANE = {
  plugin: 'crew-board',
  component: 'Pane',
  requestId: 'crew-board',
  props: {
    title: 'Crew board',
    isFocused: false,
    bodyColumns: 90,
    placement: 'dock',
    scroll: { offset: 0, bodyRows: 30 },
    view: {},
  },
} as const

test('the pane draws its header on every surface', async ($, on) => {
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ ...PANE, surface })
    const title = await ui.find({ type: 'Text', text: /CREW BOARD/ })
    expect(title?.text).toMatch(/CREW BOARD/)
    await ui.unmount()
  }
})

test('a running subagent found in the transcripts is shown with its task, tokens and last tool', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const live = (name: string) => ({ name, kind: 'file' as const, size: 500, mtimeMs: Date.now(), isLink: false })
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('/subagents')) return { value: [live('agent-abc.meta.json'), live('agent-abc.jsonl')] }
    if (p.includes('/.claude/projects/')) return { value: [dir('session-1')] }

    return { value: [] }
  })
  const usage = '"usage":{"input_tokens":2,"cache_creation_input_tokens":1000,"cache_read_input_tokens":5000,"output_tokens":400}'
  on('fs.read', (_, e) =>
    fwd(e.path).endsWith('.meta.json')
      ? { value: JSON.stringify({ agentType: 'cpp', description: 'Reproduction test', finished: false }) }
      : { value: `{"message":{"content":"work on task-319-01-reproduce now"}}\n{"message":{"content":[{"type":"tool_use","id":"t1","name":"Bash"}],${usage}}}` },
  )

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(2500)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  expect((await ui.find({ type: 'Text', text: /working/ }))?.text).toMatch(/working/)
  expect((await ui.find({ type: 'Text', text: /6k tok/ }))?.text).toMatch(/6k tok/)
  expect((await ui.find({ type: 'Text', text: /#319/ }))?.text).toMatch(/#319/)
  await ui.unmount()
})

test('the task view shows the token table and the executor cost from the usage snapshot', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const snapshot = JSON.stringify({
    records: [
      {
        kind: 'agent',
        id: 'exec1',
        role: 'pilot',
        type: 'executor',
        globalTask: '318',
        input: 10,
        cacheWrite: 2000,
        cacheRead: 100000,
        output: 1500,
        seconds: 120,
        models: ['claude-sonnet-5-5'],
      },
    ],
  })
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('/vault/scripts')) return { value: [file('usage-snapshot.json')] }
    if (p === DIR) return { value: [file('task-318-preload.md'), dir('task-318-preload')] }
    if (p.endsWith('/task-318-preload')) return { value: [file('task-318-01-preload.md')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('usage-snapshot.json')) return { value: snapshot }

    return { value: p.endsWith('/task-318-preload.md') ? TASK : SUB }
  })

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(200)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  await ui.press({ key: 't-task-318-preload' })
  expect((await ui.find({ type: 'Text', text: /Executor/ }))?.text).toMatch(/Executor/)
  // folded at first: only the summary line shows
  expect(await ui.find({ type: 'Text', text: /Cache read/ })).toBeUndefined()
  await ui.press({ key: 'costs' })
  expect((await ui.find({ type: 'Text', text: /Cache read/ }))?.text).toMatch(/Cache read/)
  expect((await ui.find({ type: 'Text', text: /sonnet/ }))?.text).toMatch(/sonnet/)
  await ui.unmount()
})

test('a usage snapshot over 4 MiB is read through its compact copy usage-agents.json', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const agents = JSON.stringify([
    { kind: 'agent', id: 'exec1', role: 'pilot', type: 'executor', globalTask: '318', input: 10, cacheWrite: 2000, cacheRead: 100000, output: 1500, seconds: 120, models: ['claude-sonnet-5-5'] },
  ])
  const big = { name: 'usage-snapshot.json', kind: 'file' as const, size: 5 * 1024 * 1024, mtimeMs: 1, isLink: false }
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('/vault/scripts')) return { value: [big, file('usage-agents.json')] }
    if (p === DIR) return { value: [file('task-318-preload.md'), dir('task-318-preload')] }
    if (p.endsWith('/task-318-preload')) return { value: [file('task-318-01-preload.md')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('usage-snapshot.json')) throw new Error('file over 4 MiB')
    if (p.endsWith('usage-agents.json')) return { value: agents }

    return { value: p.endsWith('/task-318-preload.md') ? TASK : SUB }
  })

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(200)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  await ui.press({ key: 't-task-318-preload' })
  await ui.press({ key: 'costs' })
  expect((await ui.find({ type: 'Text', text: /sonnet/ }))?.text).toMatch(/sonnet/)
  await ui.unmount()
})

test('a running planner and a big executor transcript are read through the helper script', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const big = (name: string) => ({ name, kind: 'file' as const, size: 9 * 1024 * 1024, mtimeMs: Date.now(), isLink: false })
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('/subagents')) return { value: [big('agent-p1.meta.json'), big('agent-p1.jsonl'), big('agent-x1.meta.json'), big('agent-x1.jsonl')] }
    if (p.includes('/.claude/projects/')) return { value: [dir('session-1')] }
    if (p === DIR) return { value: [file('task-318-preload.md'), dir('task-318-preload')] }
    if (p.endsWith('/task-318-preload')) return { value: [file('task-318-01-preload.md')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('agent-p1.meta.json')) return { value: JSON.stringify({ agentType: 'planner', description: 'Plan things', finished: false }) }
    if (p.endsWith('agent-x1.meta.json')) return { value: JSON.stringify({ agentType: 'executor', description: 'Execute task-318', finished: false }) }

    return { value: p.endsWith('/task-318-preload.md') ? TASK : SUB }
  })
  // the 9 MiB transcripts cannot be read by the module: the script answers for them
  on('process.run', (_, e) => {
    const planner = e.argv.join(' ').includes('agent-p1')
    const out = {
      tokens: planner ? { input: 10, cacheWrite: 0, cacheRead: 0, output: 1990 } : { input: 5, cacheWrite: 600000, cacheRead: 0, output: 5 },
      tool: 'Bash',
      head: planner ? 'plan' : 'Run ONE confirmed global task: task-318-preload',
      created: planner ? ['318'] : [],
    }

    return { value: { exitCode: 0, stdout: JSON.stringify(out), stderr: '' } }
  })

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(2500)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  await ui.press({ key: 't-task-318-preload' })
  expect((await ui.find({ type: 'Text', text: /\+2k tok running now/ }))?.text).toMatch(/running now/)
  expect((await ui.find({ type: 'Text', text: /\+600k tok running now/ }))?.text).toMatch(/running now/)
  await ui.unmount()
})

test('folders left empty are found from the mod folder, and a missing one is named in red', async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  mock.env(on, { USERPROFILE: 'C:/Users/me' })
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const asked: string[] = []
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    asked.push(p)
    // the vault is there, the transcripts folder is not
    if (p.includes('/.claude/projects/')) throw new Error('ENOENT')
    if (p.endsWith('/vault/reference/tasks')) return { value: [file('task-318-preload.md'), dir('task-318-preload')] }
    if (p.endsWith('/task-318-preload')) return { value: [file('task-318-01-preload.md')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => ({ value: fwd(e.path).endsWith('/task-318-preload.md') ? TASK : SUB }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(2500)

  // <repo>/Tools/crew-board -> <repo>/vault/reference/tasks, and ~/.claude/projects/<repo path, dashes>
  const tasks = asked.find(p => p.endsWith('/vault/reference/tasks')) ?? ''
  expect(tasks).not.toMatch(/crew-board/)
  const slug = tasks.slice(0, -'/vault/reference/tasks'.length).replace(/[^A-Za-z0-9]/g, '-')
  expect(asked).toContain(`C:/Users/me/.claude/projects/${slug}`)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  expect((await ui.find({ type: 'Text', text: /Preload the next level/ }))?.text).toMatch(/Preload/)
  expect((await ui.find({ type: 'Text', text: /transcripts folder not found/ }))?.text).toMatch(/Users.me/)
  await ui.unmount()
})

test('a missing tasks folder is named in red', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  on('fs.list', () => {
    throw new Error('ENOENT')
  })

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(200)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  expect((await ui.find({ type: 'Text', text: /tasks folder not found/ }))?.text).toMatch(/C:.Game.vault.reference.tasks/)
  await ui.unmount()
})

test('the main session is live and its cost is shared among the tasks it launched, with the weights of cost-rates.json', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const SESSION = '11111111-2222-3333-4444-555555555555'
  const agents = JSON.stringify([
    { kind: 'agent', id: 'exec1', session: '11111111', role: 'pilot', type: 'executor', globalTask: '318', input: 0, cacheWrite: 0, cacheRead: 0, output: 1000, seconds: 60, models: ['claude-opus-5-5'] },
    { kind: 'agent', id: 'exec2', session: '11111111', role: 'pilot', type: 'executor', globalTask: '400', input: 0, cacheWrite: 0, cacheRead: 0, output: 1000, seconds: 60, models: ['claude-opus-5-5'] },
    { kind: 'session', id: SESSION, session: '11111111', role: 'session', type: 'main', input: 0, cacheWrite: 0, cacheRead: 0, output: 400000, seconds: 900, models: ['claude-fable-5-2'] },
  ])
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p === PROJ) return { value: [{ name: `${SESSION}.jsonl`, kind: 'file' as const, size: 500, mtimeMs: Date.now(), isLink: false }] }
    if (p.endsWith('/vault/scripts')) return { value: [file('usage-agents.json'), file('cost-rates.json')] }
    if (p === DIR) return { value: [file('task-318-preload.md'), dir('task-318-preload')] }
    if (p.endsWith('/task-318-preload')) return { value: [file('task-318-01-preload.md')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('usage-agents.json')) return { value: agents }
    // output weighs 10 here, not the built-in 5
    if (p.endsWith('cost-rates.json')) return { value: JSON.stringify({ weights: { input: 1, cacheWrite: 1.25, cacheRead: 0.1, output: 10 } }) }
    if (p.endsWith('.jsonl')) return { value: '' }

    return { value: p.endsWith('/task-318-preload.md') ? TASK : SUB }
  })

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(2500)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  expect((await ui.find({ type: 'Text', text: /main session 11111111/ }))?.text).toMatch(/main session/)
  await ui.press({ key: 't-task-318-preload' })
  // 400k output tokens for two tasks: 200k each, 2.00 units at weight 10
  expect((await ui.find({ type: 'Text', text: /shared by 2 tasks/ }))?.text).toMatch(/fable/)
  expect((await ui.find({ type: 'Text', text: /2\.00 units · out 200k/ }))?.text).toMatch(/2\.00 units/)
  await ui.unmount()
})

test('an agent silent for ten minutes stays live while its tool call has no result yet', OPTS, async ($, on) => {
  const clock = mock.clock(on)
  on('command.register', () => ({ value: { command: 'crew' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const old = (name: string) => ({ name, kind: 'file' as const, size: 500, mtimeMs: Date.now() - 10 * 60 * 1000, isLink: false })
  on('fs.list', (_, e) => {
    const p = fwd(e.path)
    if (p.endsWith('/subagents')) return { value: [old('agent-build.meta.json'), old('agent-build.jsonl'), old('agent-idle.meta.json'), old('agent-idle.jsonl')] }
    if (p === PROJ) return { value: [dir('session-1')] }

    return { value: [] }
  })
  on('fs.read', (_, e) => ({
    value: JSON.stringify({ agentType: 'executor', description: fwd(e.path).includes('agent-build') ? 'Long build' : 'Stopped agent', finished: false }),
  }))
  on('process.run', (_, e) => ({
    value: {
      exitCode: 0,
      stdout: JSON.stringify({ tokens: { input: 1, cacheWrite: 0, cacheRead: 0, output: 1 }, tool: 'Bash', head: 'task-318-preload', created: [], pending: e.argv.join(' ').includes('agent-build') }),
      stderr: '',
    },
  }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })
  await clock.advance(2500)

  const ui = await $.ui.mount({ ...PANE, surface: 'desktop' })
  expect((await ui.find({ type: 'Text', text: /Long build/ }))?.text).toMatch(/Long build/)
  expect((await ui.find({ type: 'Text', text: /in a long tool call/ }))?.text).toMatch(/Bash/)
  expect(await ui.find({ type: 'Text', text: /Stopped agent/ })).toBeUndefined()
  await ui.unmount()
})
