export type Tok = { input: number; cacheWrite: number; cacheRead: number; output: number }

export type Usage = {
  turns: number | null
  ctxStart: number | null
  ctxPeak: number | null
  resumes: number | null
  minutes: number | null
}

export type SubView = {
  id: string
  step: number
  slug: string
  status: string
  agent: string
  model: string
  runs: number | null
  tokens: Tok | null
  tokenNote: string
  ceiling: number | null
  usage: Usage | null
  dependsOn: string
  goal: string
  result: string
  alerts: string[]
}

export type TaskRow = {
  id: string
  num: number
  title: string
  status: string
  isBlocked: boolean
  isAwaiting: boolean
  waitingFor: string
  current: string
  milestone: string
  subDone: number
  subTotal: number
  subRunning: number
  agents: string[]
  tokens: number
  units: number
  runs: number
}

export type Detail = {
  id: string
  title: string
  status: string
  milestone: string
  request: string
  goal: string
  result: string
  nextStep: string
  waitingFor: string
  subs: SubView[]
}

export type Cost = {
  tok: Tok
  runs: number
  seconds: number
  models: string[]
  shareOf: number
  ids: string[]
}

export type TaskCost = { executor: Cost | null; planner: Cost | null; relay?: Cost | null }

export type Live = {
  id: string
  type: string
  description: string
  status: string
  taskNum: number | null
  tasks: number[]
  subId: string | null
  tokens: number
  tool: string
  ageSec: number
}

export type ScanInfo = {
  phase: 'idle' | 'loading' | 'ready' | 'error'
  loaded: number
  total: number
  files: number
  at: number
  frame: number
  error: string
}

export type Nav = {
  selected: string | null
  openSub: string | null
  filter: 'all' | 'todo' | 'in-progress' | 'wait' | 'done' | 'cancel'
  page: number
  costsOpen: boolean
}

declare module 'claude-code' {
  interface PluginState {
    'crew-board': {
      rows: TaskRow[]
      live: Live[]
      scan: ScanInfo
      nav: Nav
      detail: Detail | null
      costs: Record<string, TaskCost>
    }
  }
}
