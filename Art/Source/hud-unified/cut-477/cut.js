// task-477-01: cut the bottom HUD frame pieces directly from the retained mockup scaled to 1920x1080 (Q1).
const I=require('./img.js');
const OUT=process.argv[2]; // node Art/Source/hud-unified/cut-477/cut.js <out dir>; needs ffmpeg/ffprobe on PATH and Saved/Scratch/hud-477/mockup-1080.png (mockup scaled 1920x1080 lanczos)
const m=I.load('C:/Users/soumo/Documents/Unreal Projects/RogueRigs/Saved/Scratch/hud-477/mockup-1080.png');
const L=c=>0.3*c[0]+0.59*c[1]+0.11*c[2];
const purple=c=>(c[0]-c[1]>12&&c[2]-c[1]>6);
const purpleOrBlack=c=>purple(c)||L(c)<14;
function cut(x0,y0,x1,y1,geo,isBg,sides){
  const img=I.crop(m,x0,y0,x1-x0+1,y1-y0+1);const w=img.w,h=img.h;
  const fl=new Uint8Array(w*h);const st=[];
  const seed=(x,y)=>{const k=y*w+x;if(fl[k])return;const out=!geo(x+x0,y+y0);if(out||(isBg&&isBg(I.px(img,x,y)))){fl[k]=1;st.push(k);}};
  for(let x=0;x<w;x++){if(sides.includes('t'))seed(x,0);if(sides.includes('b'))seed(x,h-1);}for(let y=0;y<h;y++){if(sides.includes('l'))seed(0,y);if(sides.includes('r'))seed(w-1,y);}
  while(st.length){const k=st.pop();const x=k%w,y=(k/w)|0;for(const[dx,dy]of[[1,0],[-1,0],[0,1],[0,-1]]){const X=x+dx,Y=y+dy;if(X>=0&&Y>=0&&X<w&&Y<h)seed(X,Y);}}
  for(let y=0;y<h;y++)for(let x=0;x<w;x++){const k=y*w+x;if(!geo(x+x0,y+y0)){img.d[k*4+3]=0;continue;}if(!fl[k])continue;
    let nb=false;for(const[dx,dy]of[[1,0],[-1,0],[0,1],[0,-1]]){const X=x+dx,Y=y+dy;if(X>=0&&Y>=0&&X<w&&Y<h&&!fl[Y*w+X])nb=true;}
    img.d[k*4+3]=(nb&&L(I.px(img,x,y))<25)?255:0;}
  return img;
}
function clear(img,ox,oy,x0,y0,x1,y1){for(let y=y0;y<=y1;y++)for(let x=x0;x<=x1;x++)img.d[((y-oy)*img.w+(x-ox))*4+3]=0;}
function fill(img,ox,oy,x0,y0,x1,y1,c){for(let y=y0;y<=y1;y++)for(let x=x0;x<=x1;x++)I.set(img,x-ox,y-oy,c);}
function interp(img,ox,oy,x0,y0,x1,y1){for(let y=y0;y<=y1;y++){const a=I.px(img,x0-1-ox,y-oy),b=I.px(img,x1+1-ox,y-oy);for(let x=x0;x<=x1;x++){const t=(x-x0+1)/(x1-x0+2);I.set(img,x-ox,y-oy,[0,1,2,3].map(i=>Math.round(a[i]*(1-t)+b[i]*t)));}}}
function trim(img){let a=img.w,b=-1,c=img.h,d=-1;for(let y=0;y<img.h;y++)for(let x=0;x<img.w;x++)if(img.d[(y*img.w+x)*4+3]>0){a=Math.min(a,x);b=Math.max(b,x);c=Math.min(c,y);d=Math.max(d,y);}return {img:I.crop(img,a,c,b-a+1,d-c+1),dx:a,dy:c};}
const oct=(X0,Y0,X1,Y1,c)=>(x,y)=>x>=X0&&x<=X1&&y>=Y0&&y<=Y1&&(x-X0)+(y-Y0)>=c&&(X1-x)+(y-Y0)>=c&&(x-X0)+(Y1-y)>=c&&(X1-x)+(Y1-y)>=c;
const R=[];const put=(name,img,X0,Y0)=>{const t=trim(img);I.save(OUT+'/'+name+'.png',t.img);R.push([name,t.img.w+'x'+t.img.h,'at',X0+t.dx,Y0+t.dy]);};
const P=JSON.parse(process.argv[3]||'{}');
// 1. health strip frame, window = green bar 647..1271 x 859..882
{const g=(x,y)=>x>=605&&x<=1316&&y>=(x>640&&x<1280?845:842)&&y<=884&&!((x-605)+(y-842)<12)&&!((1316-x)+(y-842)<12);
 const s=cut(590,836,1330,884,g,purple,'tlr');interp(s,590,836,885,853,1035,858);interp(s,590,836,885,883,1035,884);fill(s,590,836,647,857,1271,858,[8,9,9,255]);fill(s,590,836,647,883,1271,884,[8,9,9,255]);clear(s,590,836,647,859,1271,882);put('T_HUD_MockStrip',s,590,836);}
// 2. block frame, rows 885..1018, cell interiors flattened to the dark inside
{const g=(x,y)=>x>=592&&x<=1330&&y>=885&&y<=1018&&!((x-592)+(y-885)<16)&&!((1330-x)+(y-885)<16);
 const s=cut(588,885,1334,1018,g,purple,'lr');
 for(const [a,b] of [[628,791],[796,958],[963,1125],[1130,1292]]) fill(s,588,885,a+10,909,b-10,996,[18,16,18,255]);
 interp(s,588,885,885,885,1035,888);put("T_HUD_MockBlock",s,588,885);}
// 3. cell = cell 3 of the mockup, padlock removed
{const s=I.crop(m,963,900,163,108);interp(s,963,900,1003,914,1085,991);put('T_HUD_MockCell',s,963,900);}
// 4. XP rail, window 166..1861 x 1033..1057 transparent (removes the mockup ticks)
{const g=(x,y)=>(x>=140&&x<=1904&&y>=1018&&y<=1071)||oct(1846,1012,1904,1073,12)(x,y);
 const s=cut(136,1008,1910,1079,g,purpleOrBlack,'tbr');fill(s,136,1008,140,1071,1845,1071,[12,12,12,255]);for(let y=1018;y<=1070;y++)for(let x=150;x<=1845;x++)s.d[((y-1008)*s.w+(x-136))*4+3]=255;clear(s,136,1008,166,1033,1861,1057);put('T_HUD_MockRail',s,136,1008);}
// 5. Lv plate, octagon, text removed
{const g=oct(12,1012,167,1074,10);const s=cut(8,1008,172,1078,g,null,'');interp(s,8,1008,49,1027,118,1060);put('T_HUD_MockLevelPlate',s,8,1008);}
console.log(R.map(r=>r.join(' ')).join('\n'));
