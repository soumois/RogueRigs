const fs=require('fs');
const W=80,C=39.5,D=[[1,0],[-1,0],[0,1],[0,-1]];
function build(SEED,P){
let s=SEED;const rnd=()=>{s=(s*1664525+1013904223)>>>0;return s/4294967296;};
const gauss=()=>(rnd()+rnd()+rnd()-1.5)*1.15;
const g=[];for(let y=0;y<W;y++){g.push([]);for(let x=0;x<W;x++){
 const dx=x-C,dy=y-C,th=Math.atan2(dy,dx),d=Math.hypot(dx,dy);
 const r=P.base+1.6*Math.sin(3*th+1)+1.0*Math.sin(5*th+2)+0.6*Math.sin(8*th)+0.5*Math.sin(13*th+.5);
 g[y].push(d<=r&&x>=2&&x<=77&&y>=2&&y<=77?'#':' ');}}
const A=(x,y)=>x>=0&&y>=0&&x<W&&y<W&&g[y][x]!=' ';
function runs(){const res=[];
 for(const [dx,dy] of D){const vert=dx!==0;
  for(let a=0;a<W;a++){let run=[];
   for(let b=0;b<=W;b++){const x=vert?a:b,y=vert?b:a;
    const e=b<W&&A(x,y)&&!A(x+dx,y+dy);
    if(e)run.push([x,y]);else{if(run.length>6)res.push(run);run=[];}}}}
 return res;}
for(let it=0;it<800;it++){const r=runs();if(!r.length)break;
 const run=r[Math.floor(rnd()*r.length)];const m=Math.floor(run.length/2)+Math.floor((rnd()-.5)*(run.length-3));
 const n=2+Math.floor(rnd()*2);
 for(let k=0;k<n;k++){const c=run[Math.min(run.length-1,m+k)];g[c[1]][c[0]]=' ';}}
const longest=()=>Math.max(0,...runs().map(r=>r.length));
// hidden mask: cells that are hidden floor
const hid=Array.from({length:W},()=>Array(W).fill(false));
const nearHidden=(x,y)=>{for(let j=-1;j<=1;j++)for(let i=-1;i<=1;i++)if(hid[y+j]&&hid[y+j][x+i])return true;return false;};
const safe=(x,y)=>{for(let j=-1;j<=1;j++)for(let i=-1;i<=1;i++)if(!A(x+i,y+j))return false;return true;};
const total=(()=>{let t=0;g.forEach(r=>r.forEach(c=>{if(c!=' ')t++;}));return t;})();
const cnt=()=>{let r=0;g.forEach(a=>a.forEach(c=>{if(c=='#')r++;}));return r;};
// walk returns list of cells
function walk(x,y,ang,len,rho0,turn,opts={}){
 const cells=new Set();let rho=rho0;let a=ang;
 for(let k=0;k<len;k++){
  rho=Math.max(0.8,Math.min(1.6,rho+gauss()*0.12));
  const R=Math.ceil(rho)+1;
  for(let j=-R;j<=R;j++)for(let i=-R;i<=R;i++){
   const cx=Math.round(x)+i,cy=Math.round(y)+j;
   if(Math.hypot(cx-x,cy-y)<=rho&&A(cx,cy))cells.add(cx+','+cy);}
  a+=gauss()*turn+(rnd()<.06?(rnd()<.5?1:-1)*(0.8+rnd()):0);
  x+=Math.cos(a);y+=Math.sin(a);
  if(!safe(Math.round(x),Math.round(y)))break;
 }
 return [...cells].map(c=>c.split(',').map(Number)).filter(([cx,cy])=>safe(cx,cy)&&!nearHidden(cx,cy));
}
const apply=cs=>cs.forEach(([x,y])=>{if(g[y][x]=='#')g[y][x]='.';});
const floorList=()=>{const f=[];for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(g[y][x]=='.'||g[y][x]=='S')f.push([x,y]);return f;};
// spawn clearing
for(let y=0;y<W;y++)for(let x=0;x<W;x++){if(Math.hypot(x-40,y-40)<=3.6+0.5*Math.sin(x*1.7+y)&&A(x,y))g[y][x]='.';}
for(let y=38;y<=42;y++)for(let x=38;x<=42;x++)g[y][x]='.';
// main
const mainTarget=P.main*total;
let guard=0;
const floorCount=()=>total-cnt();
while(floorCount()<mainTarget&&guard++<400){
 const f=floorList();const st=f[Math.floor(rnd()*f.length)];
 apply(walk(st[0],st[1],rnd()*6.283,30+Math.floor(rnd()*50),0.9+rnd()*0.6,P.turn));
}
// main-reachable set & distance
function reach(){const rs=new Set(['40,40']);const q=[[40,40]];while(q.length){const [a,b]=q.pop();D.forEach(([i,j])=>{const x=a+i,y=b+j;if(A(x,y)&&g[y][x]!='#'&&!rs.has(x+','+y)){rs.add(x+','+y);q.push([x,y]);}});}return rs;}
// hidden pockets
let rs=reach();
const mainMan=(x,y)=>{let best=99;for(let j=-5;j<=5;j++)for(let i=-5;i<=5;i++){if(rs.has((x+i)+','+(y+j)))best=Math.min(best,Math.abs(i)+Math.abs(j));}return best;};
let pockets=0;
for(let t=0;t<4000&&pockets<P.pockets;t++){
 const x=4+rnd()*72,y=4+rnd()*72;if(!A(Math.round(x),Math.round(y))||g[Math.round(y)][Math.round(x)]!='#')continue;
 const cs=walk(x,y,rnd()*6.283,12+Math.floor(rnd()*P.plen),0.8+rnd()*0.6,P.turn+0.1);
 if(cs.length<18)continue;
 let md=99,ok=true;
 for(const [cx,cy] of cs){
  if(g[cy][cx]!='#'){ok=false;break;}
  let near=false;
  for(let j=-1;j<=1;j++)for(let i=-1;i<=1;i++)if(rs.has((cx+i)+','+(cy+j)))near=true;
  if(near){ok=false;break;}
  md=Math.min(md,mainMan(cx,cy));}
 if(!ok||md<2||md>4)continue;
 // connected check of cs itself
 cs.forEach(([cx,cy])=>{g[cy][cx]='.';hid[cy][cx]=true;});pockets++;
}
// far-cell filler
function dist(){const d=Array.from({length:W},()=>Array(W).fill(99));const q=[];
 for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(A(x,y)&&g[y][x]!='#'){d[y][x]=0;q.push([x,y]);}
 for(let h=0;h<q.length;h++){const [a,b]=q[h];D.forEach(([i,j])=>{const x=a+i,y=b+j;if(A(x,y)&&d[y][x]>d[b][a]+1){d[y][x]=d[b][a]+1;q.push([x,y]);}});}
 return d;}
function filler(){for(let it=0;it<300;it++){const d=dist();let far=[];
 for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(A(x,y)&&d[y][x]>P.maxd)far.push([x,y]);
 if(!far.length)break;
 const c=far[Math.floor(rnd()*far.length)];
 const r2=reach();let best=null,bd=1e9;
 r2.forEach(k=>{const [x,y]=k.split(',').map(Number);const dd=Math.hypot(x-c[0],y-c[1]);if(dd<bd){bd=dd;best=[x,y];}});
 apply(walk(best[0],best[1],Math.atan2(c[1]-best[1],c[0]-best[0])+gauss()*.3,Math.ceil(bd)+4+Math.floor(rnd()*5),0.8+rnd()*0.5,P.turn*0.7));
}}
filler();
// cleanup unreachable regions
{const rs2=reach();const us=new Set();
 for(let y=0;y<W;y++)for(let x=0;x<W;x++){const k=x+','+y;if(A(x,y)&&g[y][x]!='#'&&!rs2.has(k)&&!us.has(k)){const cells=[[x,y]];us.add(k);for(let h=0;h<cells.length;h++){const [a,b]=cells[h];D.forEach(([i,j])=>{const X=a+i,Y=b+j,kk=X+','+Y;if(A(X,Y)&&g[Y][X]!='#'&&!rs2.has(kk)&&!us.has(kk)){us.add(kk);cells.push([X,Y]);}});}
  let md=99;cells.forEach(([a,b])=>{for(let j=-6;j<=6;j++)for(let i=-6;i<=6;i++)if(rs2.has((a+i)+','+(b+j)))md=Math.min(md,Math.abs(i)+Math.abs(j));});
  if(cells.length<15||md-1>3)cells.forEach(([a,b])=>{g[b][a]='#';});}}}
filler();
// isolated rock removal
for(let p=0;p<3;p++)for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(g[y][x]=='#'){let n=0;D.forEach(([a,b])=>{if(A(x+a,y+b)&&g[y+b][x+a]=='#')n++;});if(n==0)g[y][x]='.';}
g[40][40]='S';
return {g,A,total,cnt,runs:longest,rs:reach,hid,rnd,dist};
}
function measure(o){
 const {g,A,total,cnt}=o;const rock=cnt();const rs=o.rs();
 const fl=total-rock;
 const seen=new Set();let pieces=0;
 for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(A(x,y)&&!seen.has(x+','+y)){pieces++;const q=[[x,y]];seen.add(x+','+y);while(q.length){const [a,b]=q.pop();D.forEach(([i,j])=>{if(A(a+i,b+j)&&!seen.has((a+i)+','+(b+j))){seen.add((a+i)+','+(b+j));q.push([a+i,b+j]);}});}}
 const d=o.dist();let maxd=0;for(let y=0;y<W;y++)for(let x=0;x<W;x++)if(A(x,y))maxd=Math.max(maxd,d[y][x]);
 // unreachable regions
 const us=new Set();const regs=[];
 for(let y=0;y<W;y++)for(let x=0;x<W;x++){const k=x+','+y;if(A(x,y)&&g[y][x]!='#'&&!rs.has(k)&&!us.has(k)){const cells=[[x,y]];us.add(k);for(let h=0;h<cells.length;h++){const [a,b]=cells[h];D.forEach(([i,j])=>{const X=a+i,Y=b+j,kk=X+','+Y;if(A(X,Y)&&g[Y][X]!='#'&&!rs.has(kk)&&!us.has(kk)){us.add(kk);cells.push([X,Y]);}});}
  let md=99;cells.forEach(([a,b])=>{for(let j=-6;j<=6;j++)for(let i=-6;i<=6;i++)if(rs.has((a+i)+','+(b+j)))md=Math.min(md,Math.abs(i)+Math.abs(j));});
  regs.push({n:cells.length,gap:md-1,cells});}}
 return {total,rock,cov:rock/total,reach:rs.size/fl,pieces,maxd,outline:o.runs(),regs};
}
module.exports={build,measure};
if(require.main===module){
 const P={base:34.5,main:0.27,turn:0.28,pockets:7,plen:14,maxd:5};
 for(let seed=1;seed<400;seed++){
  const o=build(seed,P);const m=measure(o);
  const good=m.regs.filter(r=>r.n>=15&&r.gap>=1&&r.gap<=3).length;
  if(m.pieces==1&&m.outline<=6&&m.maxd<=5&&m.total>=3600&&m.total<=4600&&m.cov>=.58&&m.cov<=.62&&m.reach>=.62&&m.reach<=.88&&good>=4){
   console.log('OK seed',seed,m.total,m.cov.toFixed(3),m.reach.toFixed(3),good,m.regs.map(r=>r.n+'/'+r.gap).join(' '));fs.writeFileSync('seed.txt',''+seed);break;}
  else if(seed<15)console.log(seed,m.total,m.cov.toFixed(3),m.reach.toFixed(3),m.pieces,m.outline,m.maxd,good);
 }
}
