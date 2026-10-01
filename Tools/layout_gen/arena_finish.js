const fs=require('fs');const {build,measure}=require('./arena_gen.js');
const P={base:34.5,main:0.27,turn:0.28,pockets:7,plen:14,maxd:5};
const seed=+fs.readFileSync('seed.txt','utf8');
const o=build(seed,P);const {g,A,rnd}=o;const D=[[1,0],[-1,0],[0,1],[0,-1]];
const rs=o.rs();const m0=measure(o);
const nrock=(x,y)=>D.filter(([a,b])=>A(x+a,y+b)&&g[y+b][x+a]=='#').length;
const ore=[];
const far=(c)=>Math.min(99,...ore.map(q=>Math.hypot(q[0]-c[0],q[1]-c[1])));
function pick(cands,n){for(let i=0;i<n;i++){let best=null,bd=-1;for(let t=0;t<300;t++){const c=cands[Math.floor(rnd()*cands.length)];const d=far(c);if(d>bd){bd=d;best=c;}}ore.push(best);g[best[1]][best[0]]='+';}}
const wall=[],pk=[];
for(let y=0;y<80;y++)for(let x=0;x<80;x++){
 if(Math.max(Math.abs(x-40),Math.abs(y-40))<=8)continue;
 if(g[y][x]=='#'&&D.some(([a,b])=>rs.has((x+a)+','+(y+b)))&&nrock(x,y)>=2)wall.push([x,y]);
 if(g[y][x]=='.'&&!rs.has(x+','+y)&&nrock(x,y)>=3)pk.push([x,y]);}
pick(pk,4);pick(wall,8);
const m=measure(o);
const cells=[];const ch={' ':'void','#':'rock','+':'heal_ore','S':'spawn'};
for(let y=0;y<80;y++)for(let x=0;x<80;x++){const t=ch[g[y][x]];if(t)cells.push({x,y,type:t});}
// iso check
let iso=0;for(let y=0;y<80;y++)for(let x=0;x<80;x++)if(g[y][x]=='#'&&!D.some(([a,b])=>A(x+a,y+b)&&(g[y+b][x+a]=='#'||g[y+b][x+a]=='+')))iso++;
// straight runs: longest run of floor with width<=4 perpendicular
let lr=0;for(let y=0;y<80;y++){let r=0;for(let x=0;x<80;x++){if(A(x,y)&&g[y][x]!='#'&&g[y][x]!='+'){r++;lr=Math.max(lr,r);}else r=0;}}
for(let x=0;x<80;x++){let r=0;for(let y=0;y<80;y++){if(A(x,y)&&g[y][x]!='#'&&g[y][x]!='+'){r++;lr=Math.max(lr,r);}else r=0;}}
const oreD=ore.map(q=>Math.hypot(q[0]-40,q[1]-40).toFixed(1));
console.log(JSON.stringify({seed,total:m.total,rock:m.rock,cov:m.cov,reach:m.reach,pieces:m.pieces,maxd:m.maxd,outline:m.outline,iso,longestFloorRun:lr,ore:ore.length,minOreDistSpawn:Math.min(...oreD)}));
console.log('regions',m.regs.map(r=>r.n+' cells gap '+r.gap).join('; '));
const out="C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Content/RogueRigs/Data/Levels/";
fs.writeFileSync(out+'arena_m1.json',JSON.stringify({version:1,name:'arena_m1',width:80,height:80,cells}));
fs.writeFileSync(out+'arena_m1.brief.json',JSON.stringify({width:80,height:80,rock_coverage:[0.57,0.63],counts:{heal_ore:[10,16],barrel:[0,0]},reachable_floor_min:0.6},null,1));
