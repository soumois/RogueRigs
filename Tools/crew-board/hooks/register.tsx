import { atom, read, update } from 'claude-code'
import type { FsEntry, Register } from 'claude-code'

import { ICON_SVG } from './icons'
import type { Cost, Detail, Live, Nav, ScanInfo, SubView, TaskCost, TaskRow, Tok, Usage } from '../types'

// What the scan needs from the engine, built in session.start where `$` is in hand.
type Io = {
  read: (path: string) => Promise<string>
  list: (path: string) => Promise<FsEntry[]>
  setRows: (rows: TaskRow[]) => Promise<unknown>
  setLive: (live: Live[]) => Promise<unknown>
  setDetail: (detail: Detail | null) => Promise<unknown>
  patchScan: (fn: (s: ScanInfo) => ScanInfo) => Promise<unknown>
  getNav: () => Promise<Nav>
  setCosts: (costs: Record<string, TaskCost>) => Promise<unknown>
  run: (argv: readonly string[]) => Promise<{ exitCode: number; stdout: string }>
  root: () => Promise<string>
  home: () => Promise<string | undefined>
}

const PANE = 'crew-board'

// The game's palette (vault color-code): gunmetal plates, company yellow, Duskite violet, teal, rust orange.
// High-contrast dark theme (owner mock-up 2026-10-27): navy plates, slate edges, saturated accents.
const C = {
  bg: '#0B1220',
  panel: '#111A2B',
  hover: '#1B2A47',
  edge: '#243049',
  yellow: '#FACC15',
  orange: '#F59E0B',
  teal: '#14B8A6',
  ok: '#22C55E',
  violet: '#A78BFA',
  cyan: '#38BDF8',
  magenta: '#EC4899',
  green: '#84CC16',
  steel: '#3B82F6',
  red: '#EF4444',
  gold: '#FDE68A',
  grey: '#8B95A7',
  white: '#E5E9F0',
}

const AGENTS: Record<string, { glyph: string; color: string }> = {
  cpp: { glyph: '⚙', color: C.steel },
  bp: { glyph: '◈', color: '#10B981' },
  art: { glyph: '✦', color: C.magenta },
  '3d': { glyph: '⬢', color: C.violet },
  'level-design': { glyph: '▦', color: C.green },
  text: { glyph: '✎', color: C.gold },
  'style-guide': { glyph: '✔', color: '#F97316' },
  doc: { glyph: '▤', color: '#FB7185' },
  planner: { glyph: '☰', color: C.cyan },
  executor: { glyph: '▶', color: C.yellow },
  orchestrator: { glyph: '♜', color: C.yellow },
  capture: { glyph: '◙', color: C.teal },
  relay: { glyph: '♜', color: C.white },
  claude: { glyph: '◉', color: C.grey },
}
const agentOf = (name: string) => AGENTS[name] ?? { glyph: '◉', color: C.grey }

// The five statuses of the board. The vault has todo, blocked, review, in-progress, done: blocked reads as To do
// (with a red tag), review as In progress; cancelled/canceled/cancel read as Cancel. Wait answers is derived, see eff().
type Norm = 'todo' | 'in-progress' | 'wait' | 'done' | 'cancel'
const STATUS: Record<Norm, { glyph: string; color: string; label: string }> = {
  todo: { glyph: '○', color: C.grey, label: 'To do' },
  'in-progress': { glyph: '▶', color: C.yellow, label: 'In progress' },
  wait: { glyph: '◐', color: C.violet, label: 'Wait answers' },
  done: { glyph: '✔', color: C.ok, label: 'Done' },
  cancel: { glyph: '✕', color: C.red, label: 'Cancel' },
}
const norm = (s: string): Norm =>
  s === 'done'
    ? 'done'
    : /^wait/.test(s)
      ? 'wait'
    : /^cancel/.test(s)
      ? 'cancel'
      : s === 'in-progress' || s === 'running' || s === 'review'
        ? 'in-progress'
        : 'todo'
const statusOf = (s: string) => STATUS[norm(s)]

// Wait answers: nobody works on it and the owner acts next, either the plan awaits the owner's confirmation or the vault
// says in progress while no live agent is on it (an agent that stopped to ask the owner something).
const eff = (r: TaskRow, isLive: boolean): Norm => {
  const n = norm(r.status)
  if (n === 'done' || n === 'cancel') return n
  if (isLive) return 'in-progress'
  if (n === 'in-progress' || r.isAwaiting) return 'wait'

  return n
}

// Weights and alert thresholds: read from vault/scripts/cost-rates.json at every scan (loadRates); these stand until then
// and when the file is missing or unreadable.
let WEIGHTS = { input: 1, cacheWrite: 1.25, cacheRead: 0.1, output: 5 }
let ALERT_TURNS = 55
let ALERT_START_CTX = 25000

const rowsAtom = atom({ plugin: 'crew-board', key: 'rows' } as const, [] as TaskRow[])
const liveAtom = atom({ plugin: 'crew-board', key: 'live' } as const, [] as Live[])
const scanAtom = atom(
  { plugin: 'crew-board', key: 'scan' } as const,
  { phase: 'idle', loaded: 0, total: 0, files: 0, at: 0, frame: 0, error: '' } as ScanInfo,
)
const navAtom = atom(
  { plugin: 'crew-board', key: 'nav' } as const,
  { selected: null, openSub: null, filter: 'all', page: 0, costsOpen: false } as Nav,
)
const costsAtom = atom({ plugin: 'crew-board', key: 'costs' } as const, {} as Record<string, TaskCost>)
const detailAtom = atom({ plugin: 'crew-board', key: 'detail' } as const, null as Detail | null)

// ---------- parsing ----------

const num = (s: string | undefined) => {
  if (s === undefined) return 0
  const v = parseFloat(s)
  return Math.round(/k$/i.test(s) ? v * 1000 : v)
}

const parseDoc = (text: string) => {
  const fm: Record<string, string> = {}
  const body: Record<string, string> = {}
  let title = ''
  let state = 0
  for (const line of text.split(/\r?\n/)) {
    if (line === '---' && state < 2) {
      state += 1
      continue
    }
    if (state === 1) {
      const m = line.match(/^(\w+):\s*(.*)$/)
      if (m) fm[m[1]] = m[2].replace(/^"(.*)"$/, '$1')
      continue
    }
    if (!title && line.startsWith('# ')) title = line.slice(2).trim()
    const b = line.match(/^- ([a-z_]+):\s*(.*)$/)
    if (b && !(b[1] in body)) body[b[1]] = b[2]
  }
  return { fm, body, title }
}

const parseTokens = (s: string): Tok | null => {
  const m = s.match(/input ([\d.]+k?), cache write ([\d.]+k?), cache read ([\d.]+k?), output ([\d.]+k?)/)
  return m ? { input: num(m[1]), cacheWrite: num(m[2]), cacheRead: num(m[3]), output: num(m[4]) } : null
}

const parseUsage = (s: string | undefined): Usage | null => {
  if (!s) return null
  const pick = (re: RegExp) => {
    const m = s.match(re)
    return m ? num(m[1]) : null
  }
  return {
    turns: pick(/turns (\d+)/),
    ctxStart: pick(/start ([\d.]+k?)/),
    ctxPeak: pick(/peak ([\d.]+k?)/),
    resumes: pick(/resumes (\d+)/),
    minutes: pick(/([\d.]+) min/),
  }
}

const ZERO: Tok = { input: 0, cacheWrite: 0, cacheRead: 0, output: 0 }
const addTok = (a: Tok, b: Tok | null): Tok =>
  b ? { input: a.input + b.input, cacheWrite: a.cacheWrite + b.cacheWrite, cacheRead: a.cacheRead + b.cacheRead, output: a.output + b.output } : a
const tokTotal = (t: Tok) => t.input + t.cacheWrite + t.cacheRead + t.output
const tokUnits = (t: Tok) =>
  (t.input * WEIGHTS.input + t.cacheWrite * WEIGHTS.cacheWrite + t.cacheRead * WEIGHTS.cacheRead + t.output * WEIGHTS.output) /
  1e6

const cleanAgent = (raw: string | undefined) =>
  (raw ?? '?').trim().split(/[\s(,]/)[0].replace(/^agent-/, '') || '?'

const parseSub = (file: string, text: string): SubView | null => {
  const { fm, body } = parseDoc(text)
  const id = fm.id ?? file.replace(/\.md$/, '')
  const m = id.match(/^task-(\d+)-(\d+)-(.*)$/)
  if (!m) return null
  const tokens = parseTokens(body.tokens ?? '')
  const ceilM = (body.token_ceiling ?? '').match(/^([\d.]+)(k?)/)
  const ceiling = ceilM ? num(ceilM[1] + ceilM[2]) : null
  const usage = parseUsage(body.usage)
  const runsN = parseInt(body.runs ?? '', 10)
  const alerts: string[] = []
  if (tokens && ceiling && tokens.input + tokens.output > ceiling) alerts.push('input+output over token_ceiling')
  if (usage?.turns && usage.turns >= ALERT_TURNS) alerts.push(`${usage.turns} turns`)
  if (usage?.ctxStart && usage.ctxStart > ALERT_START_CTX) alerts.push('start context large')
  if (usage?.resumes) alerts.push(`resumed x${usage.resumes}`)
  return {
    id,
    step: parseInt(m[2], 10),
    slug: m[3],
    status: fm.status ?? '?',
    agent: cleanAgent(body.agent),
    model: (body.model ?? '').split(/[\s(,]/)[0],
    runs: Number.isNaN(runsN) ? null : runsN,
    tokens,
    tokenNote: tokens ? '' : (body.tokens ?? ''),
    ceiling,
    usage,
    dependsOn: body.depends_on ?? '',
    goal: body.goal ?? fm.description ?? '',
    result: body.result ?? '',
    alerts,
  }
}

type TaskRec = {
  mtime: number
  title: string
  status: string
  milestone: string
  request: string
  goal: string
  result: string
  nextStep: string
  confirmed: string
  waitingFor: string
  subs: Map<string, { mtime: number; sub: SubView }>
  finished: boolean
}

const emptyRec = (): TaskRec => ({
  mtime: 0,
  title: '',
  status: 'todo',
  milestone: '',
  request: '',
  goal: '',
  result: '',
  nextStep: '',
  confirmed: '',
  waitingFor: '',
  subs: new Map(),
  finished: false,
})

const summarize = (id: string, rec: TaskRec): TaskRow => {
  const subs = [...rec.subs.values()].map(s => s.sub)
  let tokens = 0
  let units = 0
  let runs = 0
  for (const s of subs) {
    if (s.tokens) {
      tokens += tokTotal(s.tokens)
      units += tokUnits(s.tokens)
    }
    runs += s.runs ?? 0
  }
  const subRunning = subs.filter(s => s.status === 'in-progress').length
  return {
    id,
    num: parseInt(id.slice(5), 10),
    title: rec.title,
    status: norm(rec.status) === 'todo' && subRunning > 0 ? 'in-progress' : norm(rec.status),
    isBlocked: rec.status === 'blocked',
    waitingFor: rec.waitingFor,
    isAwaiting: /^no/i.test(rec.confirmed) && norm(rec.status) === 'todo',
    current: (subs.find(s => s.status === 'in-progress') ?? subs.filter(s => norm(s.status) === 'todo').sort((a, b) => a.step - b.step)[0])
      ?.slug ?? '',
    milestone: rec.milestone,
    subDone: subs.filter(s => s.status === 'done').length,
    subTotal: subs.length,
    subRunning,
    agents: [...new Set(subs.map(s => s.agent))],
    tokens,
    units,
    runs,
  }
}

const detailOf = (id: string, rec: TaskRec): Detail => ({
  id,
  title: rec.title,
  status: rec.status,
  milestone: rec.milestone,
  request: rec.request,
  goal: rec.goal,
  result: rec.result,
  nextStep: rec.nextStep,
  waitingFor: rec.waitingFor,
  subs: [...rec.subs.values()].map(s => s.sub).sort((a, b) => a.step - b.step),
})

const pool = async <T,>(items: T[], size: number, fn: (item: T) => Promise<void>) => {
  let next = 0
  const worker = async () => {
    while (next < items.length) {
      const item = items[next]
      next += 1
      await fn(item)
    }
  }
  await Promise.all(Array.from({ length: Math.min(size, items.length) }, worker))
}

// ---------- formatting ----------

const fmtN = (n: number) =>
  n >= 1e6 ? `${(n / 1e6).toFixed(n >= 1e7 ? 0 : 1)}M` : n >= 1e3 ? `${Math.round(n / 1e3)}k` : String(n)
const trunc = (s: string, w: number) => (s.length > w ? `${s.slice(0, Math.max(1, w - 1))}…` : s)
const clock = (ms: number) => {
  if (!ms) return '--:--:--'
  const d = new Date(ms)
  const p = (n: number) => String(n).padStart(2, '0')
  return `${p(d.getHours())}:${p(d.getMinutes())}:${p(d.getSeconds())}`
}
const bar = (done: number, total: number, width: number) => {
  const n = total > 0 ? Math.round((done / total) * width) : 0
  return '▰'.repeat(n) + '▱'.repeat(width - n)
}

// ---------- the module ----------

export const register: Register = (on, options) => {
  const refreshMs = Math.max(1, Number(options.refreshSeconds ?? 5)) * 1000
  const fwd = (p: string) => p.split(String.fromCharCode(92)).join('/')
  const clean = (p: unknown) => fwd(String(p ?? '')).replace(/\/+$/, '')
  // an empty option is filled in session.start from the mod's own folder (resolvePaths)
  let dir = clean(options.tasksDir)
  let scriptsDir = dir.replace(/\/reference\/tasks$/, '/scripts')
  let projDir = clean(options.transcriptsDir)
  let pathsReady: Promise<void> | undefined
  const cache = new Map<string, TaskRec>()
  let io = undefined as unknown as Io
  let isBusy = false
  let isLiveBusy = false
  let signature = ''

  // The mod sits in <repo>/Tools/crew-board: the vault is two folders up, and Claude keeps the repository's transcripts
  // under ~/.claude/projects/<repo path with every non-alphanumeric character turned into a dash>. So a moved or cloned
  // repository needs no setting.
  const resolvePaths = async () => {
    const repo = clean(await io.root()).replace(/\/[^/]+\/[^/]+$/, '')
    if (dir === '') dir = `${repo}/vault/reference/tasks`
    scriptsDir = dir.replace(/\/reference\/tasks$/, '/scripts')
    if (projDir !== '') return
    let home: string | undefined
    try {
      home = await io.home()
    } catch {
      home = undefined
    }
    if (home) projDir = `${clean(home)}/.claude/projects/${repo.replace(/[^A-Za-z0-9]/g, '-')}`
  }

  // What stops the board from showing something, one entry per source, drawn in red under the header.
  const problems = { tasks: '', live: '', costs: '' }
  let shownProblems = ''
  const problemText = () => [problems.tasks, problems.live, problems.costs].filter(Boolean).join(' · ')
  const syncProblems = async () => {
    const text = problemText()
    if (text === shownProblems) return
    shownProblems = text
    await io.patchScan(s => ({ ...s, error: text }))
  }

  const refreshTask = async (id: string, mtime: number, hasFolder: boolean) => {
    const prev = cache.get(id)
    const { fm, body, title } = parseDoc(await io.read(`${dir}/${id}.md`))
    if (fm.kind !== undefined && fm.kind !== 'task') {
      cache.set(id, { ...emptyRec(), mtime, finished: true, status: 'skip' })
      return
    }
    const rec: TaskRec = {
      mtime,
      title: title || fm.description || id,
      status: fm.status ?? 'todo',
      milestone: body.milestone ?? '',
      request: body.request ?? '',
      goal: body.goal ?? fm.description ?? '',
      result: body.result ?? '',
      nextStep: body.next_step ?? '',
      confirmed: body.confirmed_by_owner ?? '',
      waitingFor: body.waiting_for && body.waiting_for !== 'none' ? body.waiting_for : '',
      subs: prev?.subs ?? new Map(),
      finished: false,
    }
    if (hasFolder) {
      const files = (await io.list(`${dir}/${id}`)).filter(f => f.kind === 'file' && /^task-.*\.md$/.test(f.name))
      const seen = new Set<string>()
      await pool(files, 8, async f => {
        seen.add(f.name)
        const old = rec.subs.get(f.name)
        if (old && old.mtime === f.mtimeMs) return
        const sub = parseSub(f.name, await io.read(`${dir}/${id}/${f.name}`))
        if (sub) rec.subs.set(f.name, { mtime: f.mtimeMs, sub })
      })
      for (const name of [...rec.subs.keys()]) if (!seen.has(name)) rec.subs.delete(name)
    }
    rec.finished = rec.status === 'done' && [...rec.subs.values()].every(s => s.sub.status === 'done')
    cache.set(id, rec)
  }

  const publish = async () => {
    const rows = [...cache.entries()]
      .filter(([, rec]) => rec.status !== 'skip')
      .map(([id, rec]) => summarize(id, rec))
      .sort((a, b) => b.num - a.num)
    await io.setRows(rows)
    const nav = await io.getNav()
    const open = nav.selected
    if (open) {
      const rec = cache.get(open)
      await io.setDetail(rec ? detailOf(open, rec) : null)
    }
  }

  // Live agents: every subagent of every session of this project leaves agent-<id>.meta.json (type, description,
  // finished) and agent-<id>.jsonl (its transcript, written as it works) under <projects>/<project>/<session>/subagents.
  // Running = not finished and its transcript was written to recently. Works across sessions on this machine.
  const metaCache = new Map<string, { type: string; description: string; finished: boolean; mtime: number }>()
  const linkCache = new Map<string, { taskNum: number | null; subId: string | null }>()
  const STALE_MS = 3 * 60 * 1000
  const WORKING_MS = 30 * 1000
  // an agent silent for longer is still live while a tool call waits for its result (a build, a capture), up to this long
  const LONG_TOOL_MS = 60 * 60 * 1000
  // a worker's prompt names its node (task-319-03-verify-...), the executor's names the global one; the planner and the
  // generic agents only mention tasks, so they are never linked to one
  const SUB_REF = /task-(\d{3})-(\d{2})-[a-z]/
  const GLOBAL_REF = /task-(\d{3})-[a-z]/
  const UNLINKED = new Set(['planner', 'claude', 'general-purpose', 'Explore', 'Plan'])
  const linkOf = (type: string, text: string) => {
    if (UNLINKED.has(type)) return { taskNum: null, subId: null }
    const sub = text.match(SUB_REF)
    if (sub) return { taskNum: parseInt(sub[1], 10), subId: `task-${sub[1]}-${sub[2]}` }
    const g = text.match(GLOBAL_REF)

    return { taskNum: g ? parseInt(g[1], 10) : null, subId: null }
  }
  const USAGE = /"usage":\{"input_tokens":(\d+),"cache_creation_input_tokens":(\d+),"cache_read_input_tokens":(\d+),"output_tokens":(\d+)/g
  const TOOL_USE = /"type":"tool_use","id":"[^"]+","name":"([\w:-]+)"/g

  const readMeta = async (path: string) => {
    const j = JSON.parse(await io.read(path))

    return { type: String(j.agentType ?? '?'), description: String(j.description ?? ''), finished: Boolean(j.finished) }
  }

  type LiveUsage = { at: number; mtime: number; tokens: number; tool: string; head: string; created: string[]; tok: Tok | null; models: string[]; seconds: number; isPending: boolean }
  const NO_USAGE: LiveUsage = { at: 0, mtime: 0, tokens: 0, tool: '', head: '', created: [], tok: null, models: [], seconds: 0, isPending: false }
  const usageCache = new Map<string, LiveUsage>()
  const USAGE_EVERY_MS = 6000
  const RELAY_EVERY_MS = 30000
  let pluginRoot = ''

  // The helper script reads any size (the module reads 4 MiB at most); the regex pass over a small file is the fallback.
  const readUsage = async (path: string, size: number): Promise<LiveUsage | undefined> => {
    try {
      if (pluginRoot === '') pluginRoot = fwd(await io.root())
      const out = await io.run(['node', `${pluginRoot}/usage_of.js`, path])
      if (out.exitCode === 0) {
        const j = JSON.parse(out.stdout)

        return {
          at: Date.now(),
          mtime: 0,
          isPending: Boolean(j.pending),
          tokens: tokTotal(j.tokens),
          tool: String(j.tool ?? ''),
          head: String(j.head ?? ''),
          created: Array.isArray(j.created) ? j.created.map(String) : [],
          tok: j.tokens as Tok,
          models: Array.isArray(j.models) ? j.models.map(String) : [],
          seconds: Number(j.seconds ?? 0),
        }
      }
    } catch {
      // node missing or the script failed: fall through
    }
    if (size >= 4 * 1024 * 1024) return undefined
    try {
      const text = await io.read(path)
      let tokens = 0
      for (const u of text.matchAll(USAGE)) tokens += Number(u[1]) + Number(u[2]) + Number(u[3]) + Number(u[4])
      const tools = [...text.matchAll(TOOL_USE)]

      return { at: Date.now(), mtime: 0, isPending: false, tokens, tool: tools.length > 0 ? tools[tools.length - 1][1] : '', head: text.slice(0, 8000), created: [], tok: null, models: [], seconds: 0 }
    } catch {
      return undefined
    }
  }

  const pollLive = async () => {
    if (isLiveBusy) return
    isLiveBusy = true
    try {
      await pathsReady
      if (projDir === '') {
        problems.live = 'transcripts folder unknown: set the transcriptsDir option'
        await syncProblems()
        return
      }
      const now = Date.now()
      const live: Live[] = []
      let entries: FsEntry[]
      try {
        entries = await io.list(projDir)
      } catch {
        problems.live = `transcripts folder not found: ${projDir}`
        await syncProblems()
        return
      }
      problems.live = ''
      // a transcript is read again only once it has changed, and at most every `everyMs`
      const usageFor = async (key: string, path: string, log: FsEntry, everyMs: number) => {
        let usage = usageCache.get(key)
        if (!usage || (usage.mtime !== log.mtimeMs && now - usage.at > everyMs)) {
          usage = { ...((await readUsage(path, log.size)) ?? usage ?? NO_USAGE), at: now, mtime: log.mtimeMs }
          usageCache.set(key, usage)
        }

        return usage
      }
      // the main session of each conversation (the relay that launches the planner and the executors): its transcript
      // <session>.jsonl sits beside the session's folder
      for (const f of entries) {
        const m = f.kind === 'file' ? f.name.match(/^([0-9a-f]{8})-[0-9a-f-]{27}\.jsonl$/) : null
        const age = now - f.mtimeMs
        if (!m || age > STALE_MS) continue
        const usage = await usageFor(`main-${f.name}`, `${projDir}/${f.name}`, f, RELAY_EVERY_MS)
        live.push({
          id: f.name.replace(/\.jsonl$/, ''),
          type: 'relay',
          description: `main session ${m[1]}`,
          status: age < WORKING_MS ? 'running' : usage.isPending ? 'tool' : 'waiting',
          taskNum: null,
          tasks: [],
          subId: null,
          tokens: usage.tokens,
          tool: usage.tool,
          ageSec: Math.max(0, Math.round(age / 1000)),
        })
      }
      const sessions = entries.filter(e => e.kind === 'dir')
      await pool(sessions, 6, async session => {
        const folder = `${projDir}/${session.name}/subagents`
        let files: FsEntry[]
        try {
          files = await io.list(folder)
        } catch {
          return
        }
        const byId = new Map<string, { meta?: FsEntry; log?: FsEntry }>()
        for (const f of files) {
          const m = f.name.match(/^agent-(.+?)(\.meta\.json|\.jsonl)$/)
          if (!m) continue
          const slot = byId.get(m[1]) ?? {}
          if (m[2] === '.jsonl') slot.log = f
          else slot.meta = f
          byId.set(m[1], slot)
        }
        for (const [id, slot] of byId) {
          if (!slot.meta || !slot.log) continue
          let meta = metaCache.get(id)
          if (meta?.finished) continue
          const age = now - slot.log.mtimeMs
          if (age > LONG_TOOL_MS) continue
          if (!meta || age <= STALE_MS || meta.mtime !== slot.meta.mtimeMs) {
            try {
              meta = { ...(await readMeta(`${folder}/agent-${id}.meta.json`)), mtime: slot.meta.mtimeMs }
              metaCache.set(id, meta)
            } catch {
              if (!meta) continue
            }
          }
          if (meta.finished) continue
          // the transcript says which task the agent works on (its first prompt, or the tasks a planner created), how much it
          // has used (each API message once), its last tool and whether that tool still runs; usage_of.js reads it whatever
          // its size, when it changed and at most every few seconds
          const usage = await usageFor(id, `${folder}/agent-${id}.jsonl`, slot.log, USAGE_EVERY_MS)
          // silent for over 3 minutes: live only while a tool call waits for its result, else the agent stopped
          if (age > STALE_MS && !usage.isPending) continue
          let link = linkCache.get(id)
          if (!link) {
            link = linkOf(meta.type, usage.head)
            linkCache.set(id, link)
          }
          const tasks = usage.created.map(Number)
          live.push({
            id,
            type: meta.type,
            description: meta.description,
            status: age < WORKING_MS ? 'running' : usage.isPending ? 'tool' : 'waiting',
            taskNum: tasks.length > 0 ? tasks[0] : link.taskNum,
            tasks,
            subId: link.subId,
            tokens: usage.tokens,
            tool: usage.tool,
            ageSec: Math.max(0, Math.round(age / 1000)),
          })
        }
      })
      live.sort((a, b) => a.ageSec - b.ageSec)
      const sig = JSON.stringify(live.map(l => [l.id, l.status, l.tokens, l.tool, Math.floor(l.ageSec / 10)]))
      if (sig !== signature) {
        signature = sig
        await io.setLive(live)
      }
      await syncProblems()
    } catch (err) {
      problems.live = `live agents: ${err instanceof Error ? err.message : String(err)}`
      await syncProblems()
    } finally {
      isLiveBusy = false
    }
  }

  // Executor and planner costs come from the vault's usage snapshot (agent-usage.js --snapshot, refreshed by every subtask
  // close): pilots (executors) carry their global task, planning runs name the tasks they created and are split among them.
  // Agents that finished since the last snapshot are read from their transcripts, so a cost shows as soon as the agent ends.
  let usageMtime = -1
  let snapRecords: Record<string, unknown>[] = []
  const plannerTasks = new Map<string, string[]>()
  const extras = new Map<string, Record<string, unknown>>()
  const seenFinished = new Set<string>()
  const DAY_MS = 24 * 60 * 60 * 1000
  const relayRead = new Map<string, { mtime: number; at: number }>()
  const RELAY_COST_EVERY_MS = 60 * 1000

  // cost-rates.json holds the weights that turn the four token kinds into units and the alert thresholds
  let ratesMtime = -1
  const loadRates = async () => {
    try {
      const entry = (await io.list(scriptsDir)).find(f => f.name === 'cost-rates.json')
      if (!entry || entry.mtimeMs === ratesMtime) return
      const j = JSON.parse(await io.read(`${scriptsDir}/cost-rates.json`))
      const pick = (v: unknown, fallback: number) => (typeof v === 'number' && Number.isFinite(v) ? v : fallback)
      const w = j.weights ?? {}
      WEIGHTS = {
        input: pick(w.input, WEIGHTS.input),
        cacheWrite: pick(w.cacheWrite, WEIGHTS.cacheWrite),
        cacheRead: pick(w.cacheRead, WEIGHTS.cacheRead),
        output: pick(w.output, WEIGHTS.output),
      }
      ALERT_TURNS = pick(j.alerts?.turns, ALERT_TURNS)
      ALERT_START_CTX = pick(j.alerts?.startContextTokens, ALERT_START_CTX)
      ratesMtime = entry.mtimeMs
    } catch {
      // missing or unreadable: the values in hand stay
    }
  }

  const loadUsage = async () => {
    let changed = false
    // usage-agents.json: the snapshot's agent records without debug traces, written by agent-usage.js with every snapshot,
    // because the snapshot itself passed the module's 4 MiB read limit
    const scripts = await io.list(scriptsDir)
    const compact = scripts.find(f => f.name === 'usage-agents.json')
    const entry = compact ?? scripts.find(f => f.name === 'usage-snapshot.json')
    if (entry && entry.mtimeMs !== usageMtime) {
      if (compact) snapRecords = JSON.parse(await io.read(`${scriptsDir}/usage-agents.json`)) as Record<string, unknown>[]
      else if (entry.size < 4 * 1024 * 1024)
        snapRecords = ((JSON.parse(await io.read(`${scriptsDir}/usage-snapshot.json`)).records ?? []) as Record<string, unknown>[]).filter(r => r.kind === 'agent' || r.kind === 'session')
      else throw new Error('usage-snapshot.json is over 4 MiB and usage-agents.json is missing: run node vault/scripts/agent-usage.js --snapshot')
      usageMtime = entry.mtimeMs
      changed = true
      // a new snapshot holds the agents read from transcripts since the last one: read them again only if they changed after it
      extras.clear()
      seenFinished.clear()
      relayRead.clear()
      // publish what the file holds right away: the transcript pass below starts processes and may be slow
      for (const r of snapRecords) if (r.role === 'planning' && Array.isArray(r.createdTasks)) plannerTasks.set(String(r.id), (r.createdTasks as unknown[]).map(String))
      await publishCosts([])
    }
    const snapIds = new Set(snapRecords.map(r => String(r.id)))
    let top: FsEntry[] = []
    try {
      top = projDir === '' ? [] : await io.list(projDir)
    } catch {
      // a missing transcripts folder is reported by pollLive; the snapshot's costs still show
    }
    const sessions = top.filter(e => e.kind === 'dir')

    // main sessions (the relay) written to after the snapshot: read from their transcript, at most once a minute each
    for (const f of top) {
      const m = f.kind === 'file' ? f.name.match(/^([0-9a-f]{8}-[0-9a-f-]{27})\.jsonl$/) : null
      if (!m || Date.now() - f.mtimeMs > 2 * DAY_MS || f.mtimeMs <= usageMtime) continue
      const seen = relayRead.get(m[1])
      if (seen && (seen.mtime === f.mtimeMs || Date.now() - seen.at < RELAY_COST_EVERY_MS)) continue
      const got = await readUsage(`${projDir}/${f.name}`, f.size)
      relayRead.set(m[1], { mtime: f.mtimeMs, at: Date.now() })
      if (!got || !got.tok) continue
      extras.set(m[1], { kind: 'session', id: m[1], session: m[1].slice(0, 8), type: 'main', role: 'session', seconds: got.seconds, models: got.models, ...got.tok })
      changed = true
    }

    for (const session of sessions) {
      let files: FsEntry[]
      try {
        files = await io.list(`${projDir}/${session.name}/subagents`)
      } catch {
        continue
      }
      for (const f of files) {
        const m = f.name.match(/^agent-(.+)\.meta\.json$/)
        if (!m || extras.has(m[1]) || seenFinished.has(m[1])) continue
        const log = files.find(x => x.name === `agent-${m[1]}.jsonl`)
        // an agent already in the snapshot is read again only when its transcript changed after the snapshot was written
        // (it was still running then: partial tokens, no task created yet)
        if (!log || Date.now() - log.mtimeMs > 2 * DAY_MS || (snapIds.has(m[1]) && log.mtimeMs <= usageMtime)) {
          seenFinished.add(m[1])
          continue
        }
        let meta: { type: string; finished: boolean }
        try {
          const j = JSON.parse(await io.read(`${projDir}/${session.name}/subagents/${f.name}`))
          meta = { type: String(j.agentType ?? ''), finished: Boolean(j.finished) }
        } catch {
          continue
        }
        // an agent stopped by the owner is never marked finished: a transcript untouched for 10 minutes counts as ended,
        // unless a tool call still waits for its result (pollLive keeps that agent live)
        const ended = meta.finished || (Date.now() - log.mtimeMs > 10 * 60 * 1000 && usageCache.get(m[1])?.isPending !== true)
        if (!ended || (meta.type !== 'planner' && meta.type !== 'executor')) {
          if (ended) seenFinished.add(m[1])
          continue
        }
        const got = await readUsage(`${projDir}/${session.name}/subagents/${log.name}`, log.size)
        seenFinished.add(m[1])
        if (!got || !got.tok) continue
        const g = got.head.match(/task-(\d{3})-[a-z]/)
        extras.set(m[1], {
          kind: 'agent',
          id: m[1],
          session: session.name.slice(0, 8),
          type: meta.type,
          role: meta.type === 'planner' ? 'planning' : 'pilot',
          globalTask: meta.type === 'executor' && g ? g[1] : undefined,
          seconds: got.seconds,
          models: got.models,
          ...got.tok,
        })
        plannerTasks.set(m[1], got.created)
        changed = true
      }
    }
    if (!changed) return
    await publishCosts(sessions)
  }

  const publishCosts = async (sessions: FsEntry[]) => {
    const costs: Record<string, TaskCost> = {}
    const sessionTasks = new Map<string, Set<string>>()
    const add = (num: string, role: 'executor' | 'planner' | 'relay', r: Record<string, unknown>, share: number) => {
      if (role !== 'relay') sessionTasks.set(String(r.session), (sessionTasks.get(String(r.session)) ?? new Set()).add(num))
      const t = (r.input === undefined ? ZERO : r) as unknown as Tok
      const scaled: Tok = {
        input: Math.round(Number(t.input) / share),
        cacheWrite: Math.round(Number(t.cacheWrite) / share),
        cacheRead: Math.round(Number(t.cacheRead) / share),
        output: Math.round(Number(t.output) / share),
      }
      const entry2 = costs[num] ?? { executor: null, planner: null, relay: null }
      const c: Cost = entry2[role] ?? { tok: ZERO, runs: 0, seconds: 0, models: [], shareOf: 1, ids: [] }
      c.tok = addTok(c.tok, scaled)
      c.runs += 1
      c.seconds += Math.round(Number(r.seconds ?? 0) / share)
      for (const m of Array.isArray(r.models) ? (r.models as string[]) : []) {
        const short = /fable/.test(m) ? 'fable' : /opus/.test(m) ? 'opus' : /sonnet/.test(m) ? 'sonnet' : /haiku/.test(m) ? 'haiku' : m
        if (!c.models.includes(short)) c.models.push(short)
      }
      c.shareOf = Math.max(c.shareOf, share)
      c.ids.push(String(r.id))
      entry2[role] = c
      costs[num] = entry2
    }
    // a planner's tasks: the nodes it created, read from its transcript while that exists, else the tasks named in its report
    // an agent read again from its transcript replaces its snapshot record
    const records = [...snapRecords.filter(r => !extras.has(String(r.id))), ...extras.values()]
    for (const r of records) {
      const id = String(r.id)
      if (r.role !== 'planning' || plannerTasks.has(id) || sessions.length === 0) continue
      const dirName = sessions.find(d => d.name.startsWith(String(r.session)))?.name
      const found = dirName ? await readUsage(`${projDir}/${dirName}/subagents/agent-${id}.jsonl`, 0) : undefined
      plannerTasks.set(id, found ? found.created : [])
    }
    for (const r of records) {
      if (r.role === 'pilot' && r.globalTask) add(String(r.globalTask), 'executor', r, 1)
      else if (r.role === 'planning') {
        const created = plannerTasks.get(String(r.id)) ?? []
        const tasks = created.length > 0 ? created : Array.isArray(r.handbackTasks) ? (r.handbackTasks as string[]) : []
        for (const n of tasks) add(String(n), 'planner', r, tasks.length)
      }
    }
    // the relay: a main session's cost is split equally among the tasks whose planner or executor it launched; a session
    // that launched none (a discussion, tooling work) belongs to no task
    for (const r of records) {
      if (r.role !== 'session') continue
      const tasks = [...(sessionTasks.get(String(r.session)) ?? [])]
      for (const n of tasks) add(n, 'relay', r, tasks.length)
    }
    await io.setCosts(costs)
  }

  const scan = async () => {
    if (isBusy) return
    isBusy = true
    try {
      await pathsReady
      const first = cache.size === 0
      let entries: FsEntry[]
      try {
        entries = await io.list(dir)
      } catch {
        problems.tasks = `tasks folder not found: ${dir === '' ? '(no path: set the tasksDir option)' : dir}`
        shownProblems = problemText()
        await io.patchScan(s => ({ ...s, phase: 'error', error: shownProblems }))

        return
      }
      problems.tasks = ''
      await loadRates()
      const folders = new Set(entries.filter(e => e.kind === 'dir').map(e => e.name))
      // finished tasks are skipped unless their own node changed on disk; every other task is re-read, subtasks by mtime
      const toRead = entries
        .filter(e => e.kind === 'file' && /^task-\d+-.+\.md$/.test(e.name))
        .map(e => ({ id: e.name.replace(/\.md$/, ''), mtime: e.mtimeMs }))
        .filter(w => {
          const rec = cache.get(w.id)
          return !(rec && rec.finished && rec.mtime === w.mtime)
        })
      let loaded = 0
      if (first) await io.patchScan(s => ({ ...s, phase: 'loading', total: toRead.length, loaded: 0 }))
      await pool(toRead, 12, async w => {
        await refreshTask(w.id, w.mtime, folders.has(w.id))
        loaded += 1
        if (first && loaded % 40 === 0) {
          const n = loaded
          await io.patchScan(s => ({ ...s, loaded: n }))
        }
      })
      let files = 0
      for (const rec of cache.values()) files += 1 + rec.subs.size
      await publish()
      try {
        await loadUsage()
        problems.costs = ''
      } catch (err) {
        // a missing or half-written snapshot is retried at the next change; the reason is shown under the header
        usageMtime = -1
        problems.costs = `costs: ${err instanceof Error ? err.message : String(err)}`
      }
      await pollLive()
      shownProblems = problemText()
      await io.patchScan(s => ({
        ...s,
        phase: 'ready',
        loaded: toRead.length,
        total: toRead.length,
        files,
        at: Date.now(),
        frame: s.frame + 1,
        error: shownProblems,
      }))
    } catch (err) {
      problems.tasks = `tasks: ${err instanceof Error ? err.message : String(err)}`
      shownProblems = problemText()
      await io.patchScan(s => ({ ...s, phase: 'error', error: shownProblems }))
    } finally {
      isBusy = false
    }
  }

  on('session.start', async ($, e, next) => {
    io = {
      read: path => $.fs.read(path),
      list: path => $.fs.list(path),
      setRows: rows => update($, rowsAtom, () => rows),
      setLive: live => update($, liveAtom, () => live),
      setDetail: detail => update($, detailAtom, () => detail),
      patchScan: fn => update($, scanAtom, fn),
      getNav: () => read($, navAtom),
      setCosts: costs => update($, costsAtom, () => costs),
      run: argv => $.process.run(argv, { timeoutMs: 15000 }),
      root: () => Promise.resolve($.plugin.root),
      home: async () => (await $.env.get('USERPROFILE')) ?? (await $.env.get('HOME')),
    }
    pathsReady = resolvePaths()
    await $.command.register({ name: 'crew', description: 'Open the crew board: vault tasks, agents, tokens' })
    if (options.openAtStart !== false) void $.ui.open({ id: PANE, title: 'Crew board' })
    $.clock.after(50, () => void scan())
    $.clock.every(refreshMs, () => void scan())
    $.clock.every(2000, () => void pollLive())

    return next(e)
  })

  on('command.run', { command: 'crew' }, async $ => {
    await $.ui.open({ id: PANE, title: 'Crew board', focus: true })
    void scan()

    return { text: 'Crew board opened.' }
  })

  // /crew typed as a prompt reopens the pane even where the app does not list a mod's command
  on('prompt.submit', async ($, e, next) => {
    if (e.text.trim().toLowerCase() === '/crew') {
      await $.ui.open({ id: PANE, title: 'Crew board', focus: true })
      void scan()

      return { drop: 'Crew board opened.' }
    }

    return next(e)
  })

  // a button above the prompt, always there, to reopen the pane after closing it
  on('ui.render', { component: 'AbovePrompt' }, async ($, e, next) => {
    if (e.props.hasSurvey) return next(e)
    const { Box, Button, Text } = $.ui.resolve(e)

    return (
      <Box gap={1}>
        <Button key="crew-open" plain onPress={() => $.ui.open({ id: PANE, title: 'Crew board', focus: true })}>
          <Text color={C.yellow} bold>
            ▚ Crew board
          </Text>
        </Button>
      </Box>
    )
  })

  on('ui.render', { component: 'Pane', requestId: PANE }, async ($, e) => {
    const ui = $.ui.resolve(e)
    const { Box, Text, Button } = ui
    const Svg = 'Svg' in ui ? ui.Svg : undefined
    const select = async (id: string | null) => {
      await update($, navAtom, n => ({ ...n, selected: id, openSub: null }))
      const rec = id ? cache.get(id) : undefined
      await update($, detailAtom, () => (id && rec ? detailOf(id, rec) : null))
    }
    const [rows, live, scanInfo, nav, detail, costs] = await Promise.all([
      read($, rowsAtom),
      read($, liveAtom),
      read($, scanAtom),
      read($, navAtom),
      read($, detailAtom),
      read($, costsAtom),
    ])
    const cols = Math.max(40, e.props.bodyColumns ?? 80)
    const bodyRows = e.props.scroll?.bodyRows ?? 26

    const chip = (text: string, color: string) => (
      <Text backgroundColor={color} color={C.bg} bold>
        {' '}
        {text}{' '}
      </Text>
    )
    // the agent's mascot (pixel-art SVG) where the surface draws one, else its glyph in the agent's colour
    const icon = (name: string, size: number) => {
      const a = agentOf(name)
      const svg = ICON_SVG[name] ?? ICON_SVG.claude
      return Svg && svg ? (
        <Svg alt={name} width={size} height={size} source={svg} />
      ) : (
        <Text color={a.color} bold>
          {a.glyph}
        </Text>
      )
    }
    const agentTag = (name: string) => (
      <Text color={agentOf(name).color} bold>
        {name}
      </Text>
    )
    // the whole block is the click target: transparent full-width rows laid over it (not on the terminal, which would blank it)
    const hit = (key: string, onPress: () => void) =>
      e.surface === 'terminal' ? null : (
        <Box position="absolute" top={0} left={0} right={0} bottom={0} flexDirection="column" overflow="hidden">
          {[0, 1, 2, 3].map(i => (
            <Button key={`${key}-${i}`} plain onPress={onPress}>
              {String.fromCharCode(160).repeat(cols)}
            </Button>
          ))}
        </Box>
      )

    const liveRow = (l: Live, sub = '') => (
      <Box gap={1} alignItems="center">
        {icon(l.type, 28)}
        <Box flexDirection="column">
          <Text>
            {agentTag(l.type)}
            <Text color={C.white}> {trunc(l.description || (l.taskNum !== null ? `task-${l.taskNum}` : l.id), Math.max(12, cols - 24))}</Text>
          </Text>
          <Text color={C.grey}>
            <Text color={l.status === 'waiting' ? C.yellow : C.cyan}>
              {l.status === 'running' ? '● working' : l.status === 'tool' ? '● in a long tool call' : '◐ waiting'}
            </Text>
            {l.tool !== '' ? ` · ${l.tool}` : ''}
            <Text color={C.orange}>
              {l.tokens > 0
                ? l.tasks.length > 1
                  ? ` · ${fmtN(l.tokens)} tok for ${l.tasks.length} tasks, ${fmtN(Math.round(l.tokens / l.tasks.length))} each`
                  : ` · ${fmtN(l.tokens)} tok`
                : ''}
            </Text>
            {` · ${l.ageSec}s ago`}
            <Text color={C.yellow}>{l.taskNum !== null ? ` · #${l.taskNum}` : ''}</Text>
            <Text color={C.white}>{sub !== '' ? ` · ${sub}` : ''}</Text>
          </Text>
        </Box>
      </Box>
    )

    const liveByTask = new Map<number, Live[]>()
    for (const l of live) {
      for (const n of new Set([...(l.taskNum === null ? [] : [l.taskNum]), ...l.tasks])) liveByTask.set(n, [...(liveByTask.get(n) ?? []), l])
    }

    // ----- header: title, counters, live agents -----
    const counts = { todo: 0, 'in-progress': 0, wait: 0, done: 0, cancel: 0 }
    const taskCost = (r: TaskRow) => {
      const c = costs[String(r.num)]
      const parts = [c?.executor, c?.planner, c?.relay]
      const tok = parts.reduce((a, p) => a + (p ? tokTotal(p.tok) : 0), 0)
      const units = parts.reduce((a, p) => a + (p ? tokUnits(p.tok) : 0), 0)

      return { tokens: r.tokens + tok, units: r.units + units }
    }
    let totalUnits = 0
    for (const r of rows) {
      totalUnits += taskCost(r).units
      counts[eff(r, liveByTask.has(r.num))] += 1
    }

    const spinner = scanInfo.frame % 2 === 0 ? '●' : '○'
    const header = (
      <Box flexDirection="column" borderStyle="round" borderColor={C.yellow} backgroundColor={C.panel} paddingX={1}>
        <Box justifyContent="space-between">
          <Text color={C.yellow} bold>
            ▚ ROGUE RIGS / CREW BOARD
          </Text>
          <Text color={live.length > 0 ? C.cyan : C.grey}>
            {spinner} {live.length} live
          </Text>
        </Box>
        <Box gap={2}>
          <Text color={C.grey}>○ {counts.todo} to do</Text>
          <Text color={C.yellow}>▶ {counts['in-progress']} in progress</Text>
          <Text color={C.violet}>◐ {counts.wait} wait answers</Text>
          <Text color={C.ok}>✔ {counts.done} done</Text>
          <Text color={C.red}>✕ {counts.cancel} cancel</Text>
          <Text color={C.orange}>Σ {totalUnits.toFixed(1)} units</Text>
        </Box>
        <Text color={C.grey} dimColor>
          {scanInfo.phase === 'loading'
            ? `reading the vault… ${scanInfo.loaded}/${scanInfo.total}`
            : `updated ${clock(scanInfo.at)} · every ${Math.round(refreshMs / 1000)}s · ${scanInfo.files} nodes cached`}
        </Text>
        {scanInfo.error !== '' && (
          <Text color={C.red} bold>
            ⚠ {scanInfo.error}
          </Text>
        )}
      </Box>
    )

    // ----- detail view -----
    if (nav.selected && detail) {
      const taskNum = parseInt(detail.id.slice(5), 10)
      const taskLive = liveByTask.get(taskNum) ?? []
      const row = rows.find(r => r.id === detail.id)
      const st = row ? STATUS[eff(row, taskLive.length > 0)] : statusOf(detail.status)
      const cost = costs[String(taskNum)] ?? { executor: null, planner: null, relay: null }
      const relayTok = cost.relay?.tok ?? ZERO
      const workers = detail.subs.reduce((a, s) => addTok(a, s.tokens), ZERO)
      const workerRuns = detail.subs.reduce((a, s) => a + (s.runs ?? 0), 0)
      const execTok = cost.executor?.tok ?? ZERO
      const planTok = cost.planner?.tok ?? ZERO
      const allTok = addTok(addTok(addTok(workers, execTok), planTok), relayTok)
      const runsOf = (c: { runs: number } | null) => (c ? c.runs : 0)
      const liveExec = taskLive.filter(l => l.type === 'executor' && !(cost.executor?.ids ?? []).includes(l.id))
      const liveExecTokens = liveExec.reduce((a, l) => a + l.tokens, 0)
      const livePlan = taskLive.filter(l => l.type === 'planner' && !(cost.planner?.ids ?? []).includes(l.id))
      const livePlanTokens = livePlan.reduce((a, l) => a + Math.round(l.tokens / Math.max(1, l.tasks.length)), 0)
      const subName = (l: Live) => {
        const s = l.subId === null ? undefined : detail.subs.find(x => x.id === l.subId || x.id.startsWith(`${l.subId}-`))

        return s ? `${String(s.step).padStart(2, '0')} ${s.slug}` : ''
      }
      const num7 = (n: number) => fmtN(n).padStart(8)
      const kinds: [string, keyof Tok][] = [
        ['Input', 'input'],
        ['Cache write', 'cacheWrite'],
        ['Cache read', 'cacheRead'],
        ['Output', 'output'],
      ]
      const tableRow = (label: string, cells: string[], color: string, isBold = false) => (
        <Text mono color={color} bold={isBold}>
          {label.padEnd(12)}
          {cells.join('')}
        </Text>
      )
      const roleCard = (name: string, agent: string, c: Cost | null, liveTokens: number, empty: string, unit = 'launches') => (
        <Box borderStyle="round" borderColor={C.edge} backgroundColor={C.panel} paddingX={1} gap={1} alignItems="center">
          {icon(agent, 34)}
          <Box flexDirection="column" flexGrow={1}>
            <Text>
              <Text color={agentOf(agent).color} bold>
                {name}
              </Text>
              <Text color={C.grey}>
                {c ? ` ${c.models.join(', ')}${c.shareOf > 1 ? ` · 1/${c.shareOf} of ~${fmtN(tokTotal(c.tok) * c.shareOf)} tok (shared by ${c.shareOf} tasks)` : ''}` : ` ${empty}`}
              </Text>
            </Text>
            <Text color={C.grey}>
              {c ? `${tokUnits(c.tok).toFixed(2)} units · out ${fmtN(c.tok.output)}${unit === 'launches' ? ` · ${Math.round(c.seconds / 60)} min` : ''}` : ''}
              <Text color={C.cyan}>{liveTokens > 0 ? ` · +${fmtN(liveTokens)} tok running now` : ''}</Text>
            </Text>
          </Box>
          <Box flexDirection="column" alignItems="flex-end">
            <Text color={C.orange}>{c ? `${fmtN(tokTotal(c.tok))} tok` : '-'}</Text>
            <Text color={C.violet}>
              {runsOf(c)} {unit}
            </Text>
          </Box>
        </Box>
      )

      return (
        <Box flexDirection="column" gap={1} backgroundColor={C.bg} padding={1} minHeight={bodyRows}>
          {header}
          <Box gap={2} alignItems="flex-start">
            <Button key="back" variant="primary" onPress={() => select(null)}>
              ← tasks
            </Button>
            {chip(`${st.glyph} ${st.label}`, st.color)}
            <Box flexGrow={1} flexShrink={1}>
              <Text color={C.white} size="heading" bold>
                <Text color={C.yellow}>#{taskNum} </Text>
                {detail.title}
              </Text>
            </Box>
          </Box>
          <Box flexDirection="column" gap={1} borderStyle="round" borderColor={C.edge} backgroundColor={C.panel} paddingX={1}>
            <Box flexDirection="column">
              <Text color={C.grey}>Description :</Text>
              <Text color={C.white}>{trunc(detail.goal, 500)}</Text>
            </Box>
            {detail.waitingFor !== '' && <Text color={C.violet}>Waiting for you: {trunc(detail.waitingFor, 300)}</Text>}
            {detail.nextStep !== '' && detail.nextStep !== 'none' && <Text color={C.yellow}>next: {trunc(detail.nextStep, 300)}</Text>}
            {norm(detail.status) !== 'done' && taskLive.length > 0 && (
              <Box flexDirection="column" gap={1}>
                <Text color={C.grey}>Running now</Text>
                {taskLive.map(l => liveRow(l, subName(l)))}
              </Box>
            )}
          </Box>
          <Box flexDirection="column" borderStyle="round" borderColor={C.edge} backgroundColor={C.panel} paddingX={1}>
            <Box gap={2} alignItems="center">
              <Button key="costs" variant="primary" onPress={() => update($, navAtom, n => ({ ...n, costsOpen: !n.costsOpen }))}>
                {nav.costsOpen ? '▾' : '▸'}
              </Button>
              <Text color={C.orange} bold>
                Σ {fmtN(tokTotal(allTok))} tokens
              </Text>
              <Text color={C.orange}>{tokUnits(allTok).toFixed(2)} units</Text>
              <Text color={C.violet}>{workerRuns + runsOf(cost.executor) + runsOf(cost.planner)} launches</Text>
              <Text color={C.teal}>
                {detail.subs.filter(s => s.status === 'done').length}/{detail.subs.length} steps
              </Text>
            </Box>
            {nav.costsOpen && (
              <Box flexDirection="column" paddingTop={1}>
            {tableRow('Tokens', ['Workers', 'Executor', 'Planner', 'Relay', 'Total'].map(h => h.padStart(9)), C.grey)}
            {kinds.map(([label, key]) =>
              tableRow(
                label,
                [workers[key], execTok[key], planTok[key], relayTok[key], allTok[key]].map(n => num7(n).padStart(9)),
                key === 'output' ? C.steel : C.white,
              ),
            )}
            {tableRow(
              'Total',
              [tokTotal(workers), tokTotal(execTok), tokTotal(planTok), tokTotal(relayTok), tokTotal(allTok)].map(n => num7(n).padStart(9)),
              C.orange,
              true,
            )}
            {tableRow(
              'Units',
              [workers, execTok, planTok, relayTok, allTok].map(t => tokUnits(t).toFixed(2).padStart(9)),
              C.orange,
            )}
            {tableRow(
              'Launches',
              [String(workerRuns), String(runsOf(cost.executor)), String(runsOf(cost.planner)), '-', String(workerRuns + runsOf(cost.executor) + runsOf(cost.planner))].map(n =>
                n.padStart(9),
              ),
              C.violet,
            )}
              </Box>
            )}
          </Box>
          {roleCard('Executor', 'executor', cost.executor, liveExecTokens, 'not run yet')}
          {(cost.planner || livePlan.length > 0) && roleCard('Planner', 'planner', cost.planner, livePlanTokens, 'running')}
          {cost.relay && roleCard('Relay (main session)', 'relay', cost.relay, 0, '', 'sessions')}
          {detail.subs.length === 0 && <Text color={C.grey}>no subtask</Text>}
          {detail.subs.map(s => {
            // an agent linked to the whole task works on its in-progress step; one linked to a step, on that step
            const isLive =
              norm(s.status) !== 'done' &&
              taskLive.some(l => (l.subId === null ? norm(s.status) === 'in-progress' : s.id === l.subId || s.id.startsWith(`${l.subId}-`)))
            const sst = statusOf(isLive ? 'in-progress' : norm(s.status) === 'in-progress' ? 'wait' : s.status)
            const a = agentOf(s.agent)
            const isOpen = nav.openSub === s.id
            const tokText = s.tokens ? `${fmtN(tokTotal(s.tokens))} tok · ${tokUnits(s.tokens).toFixed(2)} units` : s.tokenNote.startsWith('open') ? 'tok open' : 'tok n/a'
            const toggle = () => update($, navAtom, n => ({ ...n, openSub: n.openSub === s.id ? null : s.id }))
            return (
              <Box
                key={`card-${s.id}`}
                flexDirection="column"
                position="relative"
                borderStyle="round"
                borderColor={isLive ? C.cyan : C.edge}
                backgroundColor={C.panel}
                hover={{ backgroundColor: C.hover, borderColor: C.cyan }}
                paddingX={1}
              >
                <Box gap={1} alignItems="center">
                  {icon(s.agent, 34)}
                  <Box flexDirection="column" flexGrow={1}>
                    <Text>
                      <Text color={C.grey}>{String(s.step).padStart(2, '0')} </Text>
                      <Text color={C.white} bold>
                        {trunc(s.slug, cols - 16)}
                      </Text>
                    </Text>
                    <Text>
                      {agentTag(s.agent)}
                      <Text color={C.grey}> {s.model !== '' ? `${s.model} · ` : ''}</Text>
                      <Text color={sst.color}>{sst.label}</Text>
                      <Text color={C.cyan}>{isLive ? ' · working now' : ''}</Text>
                      <Text color={C.red}>{s.alerts.length > 0 ? ' ⚠' : ''}</Text>
                    </Text>
                  </Box>
                  <Box flexDirection="column" alignItems="flex-end">
                    <Text color={C.orange}>{tokText}</Text>
                    <Text color={C.violet}>{s.runs ?? "?"} launches</Text>
                  </Box>
                  <Button key={`s-${s.id}`} variant="primary" onPress={toggle}>
                    {isOpen ? '▾' : '▸'}
                  </Button>
                </Box>
                {isOpen && (
                  <Box flexDirection="column" gap={1} paddingTop={1}>
                    <Box flexDirection="column">
                      <Text color={C.grey}>Goal</Text>
                      <Text color={C.white}>{trunc(s.goal, 400)}</Text>
                    </Box>
                    {s.tokens && (
                      <Box flexDirection="column">
                        <Text color={C.grey}>Tokens of this step</Text>
                        <Text color={C.orange}>
                          in {fmtN(s.tokens.input)} · cache write {fmtN(s.tokens.cacheWrite)} · cache read{' '}
                          {fmtN(s.tokens.cacheRead)} · out {fmtN(s.tokens.output)}
                        </Text>
                        <Text color={C.orange}>{tokUnits(s.tokens).toFixed(2)} units (weighted)</Text>
                        {s.ceiling !== null && (
                          <Text color={s.tokens.input + s.tokens.output > s.ceiling ? C.red : C.teal}>
                            ceiling (input + output) {bar(Math.min(s.tokens.input + s.tokens.output, s.ceiling), s.ceiling, 14)}{' '}
                            {fmtN(s.tokens.input + s.tokens.output)} / {fmtN(s.ceiling)}
                          </Text>
                        )}
                      </Box>
                    )}
                    {s.usage && (
                      <Box flexDirection="column">
                        <Text color={C.grey}>Run</Text>
                        <Text color={C.steel}>
                          {s.usage.turns ?? '?'} turns · {s.usage.minutes ?? '?'} min · resumed {s.usage.resumes ?? 0}x
                        </Text>
                        <Text color={C.steel}>
                          context {s.usage.ctxStart !== null ? fmtN(s.usage.ctxStart) : '?'} at start, {s.usage.ctxPeak !== null ? fmtN(s.usage.ctxPeak) : '?'} at peak
                        </Text>
                      </Box>
                    )}
                    {s.dependsOn !== '' && s.dependsOn !== 'none' && (
                      <Box flexDirection="column">
                        <Text color={C.grey}>Runs after</Text>
                        <Text color={C.white}>{trunc(s.dependsOn, 200)}</Text>
                      </Box>
                    )}
                    {s.alerts.length > 0 && <Text color={C.red}>⚠ {s.alerts.join(' · ')}</Text>}
                    {s.result !== '' && s.result !== 'open' && (
                      <Box flexDirection="column">
                        <Text color={C.grey}>Result</Text>
                        <Text color={C.gold}>{trunc(s.result, 500)}</Text>
                      </Box>
                    )}
                  </Box>
                )}
                {hit(`h-${s.id}`, toggle)}
              </Box>
            )
          })}
          {detail.result !== '' && detail.result !== 'open' && (
            <Text color={C.gold} dimColor>
              {trunc(detail.result, 500)}
            </Text>
          )}
        </Box>
      )
    }

    // ----- list view: one card per task -----
    const filtered = rows.filter(r => {
      return nav.filter === 'all' || eff(r, liveByTask.has(r.num)) === nav.filter
    })
    const pageSize = Math.max(3, Math.floor((bodyRows - 10) / 5))
    const pages = Math.max(1, Math.ceil(filtered.length / pageSize))
    const page = Math.min(nav.page, pages - 1)
    const slice = filtered.slice(page * pageSize, (page + 1) * pageSize)

    // every button is primary: the secondary look is unreadable on the dark plates; the selected filter carries a dot
    const filterBtn = (key: Nav['filter'], label: string, count: number) => (
      <Button
        key={`f-${key}`}
        variant="primary"
        onPress={() => update($, navAtom, n => ({ ...n, filter: key, page: 0 }))}
      >
        {`${nav.filter === key ? '● ' : ''}${label} ${count}`}
      </Button>
    )

    return (
      <Box flexDirection="column" gap={1} backgroundColor={C.bg} padding={1} minHeight={bodyRows}>
        {header}
        {live.slice(0, 5).map(l => liveRow(l))}
        <Box gap={1} flexWrap="wrap">
          {filterBtn('all', 'All', rows.length)}
          {filterBtn('todo', 'To do', counts.todo)}
          {filterBtn('in-progress', 'In progress', counts['in-progress'])}
          {filterBtn('wait', 'Wait', counts.wait)}
          {filterBtn('done', 'Done', counts.done)}
          {filterBtn('cancel', 'Cancel', counts.cancel)}
        </Box>
        <Box gap={2}>
          <Button key="prev" variant="primary" onPress={() => update($, navAtom, n => ({ ...n, page: Math.max(0, page - 1) }))}>
            ◀
          </Button>
          <Text color={C.grey}>
            {page + 1}/{pages} · {filtered.length} tasks
          </Text>
          <Button
            key="next"
            variant="primary"
            onPress={() => update($, navAtom, n => ({ ...n, page: Math.min(pages - 1, page + 1) }))}
          >
            ▶
          </Button>
        </Box>
        {slice.length === 0 && (
          <Text color={C.grey} dimColor>
            {scanInfo.phase === 'loading' ? 'loading…' : 'no task'}
          </Text>
        )}
        {slice.map(r => {
          const isLive = liveByTask.has(r.num)
          const st = STATUS[eff(r, isLive)]
          const barColor = eff(r, isLive) === 'in-progress' ? C.orange : st.color
          const total = taskCost(r)
          const open = () => select(r.id)
          return (
            <Box
              key={`card-${r.id}`}
              position="relative"
              borderStyle="round"
              borderColor={isLive ? C.cyan : C.edge}
              backgroundColor={C.panel}
              hover={{ backgroundColor: C.hover, borderColor: C.cyan }}
              paddingX={1}
              gap={1}
            >
              <Box width={3} alignItems="center" justifyContent="center">
                <Text color={st.color} size="title" bold>
                  {isLive ? '●' : st.glyph}
                </Text>
              </Box>
              <Box flexDirection="column" flexGrow={1}>
                <Text>
                  <Text color={C.yellow} bold>
                    #{String(r.num).padStart(3, '0')}{' '}
                  </Text>
                  <Text color={C.white} bold>
                    {trunc(r.title, 160)}
                  </Text>
                </Text>
                <Text>
                  <Text color={st.color}>{st.label}</Text>
                  <Text color={C.cyan}>{isLive ? ' · live' : ''}</Text>
                  <Text color={C.red}>{r.isBlocked ? ' · blocked' : ''}</Text>
                </Text>
                {r.waitingFor !== '' && <Text color={C.violet}>{trunc(r.waitingFor, 140)}</Text>}
                <Text>
                  <Text color={barColor}>{bar(r.subDone, r.subTotal, 14)} </Text>
                  <Text color={C.white}>
                    {r.subDone}/{r.subTotal} subtasks
                  </Text>
                  <Text color={C.orange}>{total.tokens > 0 ? ` · ${fmtN(total.tokens)} tok · ${total.units.toFixed(2)} units` : ''}</Text>
                </Text>
              </Box>
              <Box alignItems="center">
                <Button key={`t-${r.id}`} variant="primary" onPress={open}>
                  ›
                </Button>
              </Box>
              {hit(`h-${r.id}`, open)}
            </Box>
          )
        })}
      </Box>
    )
  })
}
