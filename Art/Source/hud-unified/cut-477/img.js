const { spawnSync } = require('child_process');
const FF = 'ffmpeg';
function size(p){ const r=spawnSync(FF,['-v','error','-i',p,'-f','null','-'],{encoding:'utf8'}); const r2=spawnSync('ffprobe',['-v','error','-select_streams','v:0','-show_entries','stream=width,height','-of','csv=p=0',p],{encoding:'utf8'}); const [w,h]=r2.stdout.trim().split(',').map(Number); return {w,h}; }
function load(p){ const {w,h}=size(p); const r=spawnSync(FF,['-v','error','-i',p,'-f','rawvideo','-pix_fmt','rgba','-'],{maxBuffer:1<<30}); return {w,h,d:Buffer.from(r.stdout)}; }
function save(p,img){ const r=spawnSync(FF,['-v','error','-y','-f','rawvideo','-pix_fmt','rgba','-s',img.w+'x'+img.h,'-i','-','-pix_fmt','rgba',p],{input:img.d}); if(r.status) throw new Error(r.stderr.toString()); }
function crop(img,x,y,w,h){ const d=Buffer.alloc(w*h*4); for(let j=0;j<h;j++) img.d.copy(d,j*w*4,((y+j)*img.w+x)*4,((y+j)*img.w+x+w)*4); return {w,h,d}; }
function px(img,x,y){ const i=(y*img.w+x)*4; return [img.d[i],img.d[i+1],img.d[i+2],img.d[i+3]]; }
function set(img,x,y,c){ const i=(y*img.w+x)*4; img.d[i]=c[0];img.d[i+1]=c[1];img.d[i+2]=c[2];img.d[i+3]=c[3]; }
module.exports={load,save,crop,px,set,size};
