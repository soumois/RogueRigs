// USD at Anthropic's API list prices, per model: the one unit that compares runs made on different models.
// Prices live in cost-rates.json "prices" (source and date there). Used by agent-usage.js and by the crew-board mod's
// usage_of.js. A list price is not a bill: on a subscription it is what the same tokens would cost through the API.
const fs = require('fs'), path = require('path');

const load = file => JSON.parse(fs.readFileSync(file || path.join(__dirname, 'cost-rates.json'), 'utf8')).prices;
const familyOf = m => /fable|mythos/.test(m || '') ? 'fable' : /opus/.test(m || '') ? 'opus' : /sonnet/.test(m || '') ? 'sonnet' : /haiku/.test(m || '') ? 'haiku' : null;
// exact model id first (a dated id like claude-haiku-4-5-20251001 drops its date), then the family's current model
const rateOf = (prices, model) => {
  const id = String(model || '').replace(/-\d{8}$/, '');
  return (prices.models && prices.models[id]) || (prices.families && prices.families[familyOf(id)]) || null;
};
const cost = (prices, r, input, write5m, write1h, read, output, promptTokens) => {
  const m = prices.multipliers;
  const base = r.over && promptTokens > r.over.promptTokens ? r.over.input : r.input;
  const readX = r.cacheRead === undefined ? m.cacheRead : r.cacheRead;
  return base * (input + write5m * m.cacheWrite5m + write1h * m.cacheWrite1h + read * readX + output * m.output) / 1e6;
};

// One API message: `u` is its usage object (cache_creation says how much of the cache write is 5-minute and 1-hour).
// null when the model has no price.
function usdOfUsage(prices, model, u) {
  const r = rateOf(prices, model);
  if (!r) return null;
  const write = u.cache_creation_input_tokens || 0;
  const write1h = Math.min(write, (u.cache_creation && u.cache_creation.ephemeral_1h_input_tokens) || 0);
  const input = u.input_tokens || 0, read = u.cache_read_input_tokens || 0;
  return cost(prices, r, input, write - write1h, write1h, read, u.output_tokens || 0, input + write + read);
}

// Summed tokens of one model family ({ input, cacheWrite, cacheRead, output }), for figures saved before the per-message
// price existed: the family's current model, and one cache duration for every write (1 hour for a main session, 5 minutes
// for an agent, as the transcripts show). null when the family has no price.
function usdOfTokens(prices, family, t, isOneHour) {
  const r = prices.families && prices.families[family];
  if (!r) return null;
  return cost(prices, r, t.input || 0, isOneHour ? 0 : t.cacheWrite || 0, isOneHour ? t.cacheWrite || 0 : 0, t.cacheRead || 0, t.output || 0, 0);
}

module.exports = { load, familyOf, rateOf, usdOfUsage, usdOfTokens };
