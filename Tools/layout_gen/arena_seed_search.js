const fs=require('fs');const {build,measure}=require('./arena_gen.js');
const P={base:34.5,main:0.27,turn:0.28,pockets:7,plen:14,maxd:5};
const fl=(g,A,x,y)=>A(x,y)&&g[y][x]!='#';
function lr(o){const {g,A}=o;let best=0;
 // a cell is "corridor-ish" horizontally if the vertical floor run through it <=4
 const vw=(x,y)=>{let n=1,j=y-1;while(fl(g,A,x,j)){n++;j--;}j=y+1;while(fl(g,A,x,j)){n++;j++;}return n;};
 const hw=(x,y)=>{let n=1,i=x-1;while(fl(g,A,i,y)){n++;i--;}i=x+1;while(fl(g,A,i,y)){n++;i++;}return n;};
 for(let y=0;y<80;y++){let r=0;for(let x=0;x<80;x++){if(fl(g,A,x,y)&&vw(x,y)<=4){r++;best=Math.max(best,r);}else r=0;}}
 for(let x=0;x<80;x++){let r=0;for(let y=0;y<80;y++){if(fl(g,A,x,y)&&hw(x,y)<=4){r++;best=Math.max(best,r);}else r=0;}}
 return best;}
let res=[];
for(let seed=1;seed<600;seed++){const o=build(seed,P);const m=measure(o);
 const good=m.regs.filter(r=>r.n>=15&&r.gap>=1&&r.gap<=3).length;
 if(m.pieces==1&&m.outline<=6&&m.maxd<=5&&m.total>=3600&&m.total<=4600&&m.cov>=.585&&m.cov<=.615&&m.reach>=.65&&m.reach<=.86&&good>=4&&m.regs.length==good){res.push([lr(o),seed]);}}
res.sort((a,b)=>a[0]-b[0]);console.log(res.slice(0,8).join(' | '));
if(res.length)fs.writeFileSync('seed.txt',''+res[0][1]);
