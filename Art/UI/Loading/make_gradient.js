// 8 x 256 vertical gradient: transparent at top to dark violet-black alpha 0.92 at the bottom (smoothstep).
const zlib=require('zlib'),fs=require('fs');const W=8,H=256;
const crcT=[];for(let n=0;n<256;n++){let c=n;for(let k=0;k<8;k++)c=c&1?0xedb88320^(c>>>1):c>>>1;crcT[n]=c>>>0;}
const crc=b=>{let c=0xffffffff;for(const x of b)c=crcT[(c^x)&255]^(c>>>8);return (c^0xffffffff)>>>0;};
const chunk=(t,d)=>{const l=Buffer.alloc(4);l.writeUInt32BE(d.length);const td=Buffer.concat([Buffer.from(t),d]);const c=Buffer.alloc(4);c.writeUInt32BE(crc(td));return Buffer.concat([l,td,c]);};
const raw=Buffer.alloc((W*4+1)*H);for(let y=0;y<H;y++){const t=y/(H-1);const s=t*t*(3-2*t);const a=Math.round(235*s);raw[y*(W*4+1)]=0;for(let x=0;x<W;x++){const o=y*(W*4+1)+1+x*4;raw[o]=8;raw[o+1]=5;raw[o+2]=16;raw[o+3]=a;}}
const ih=Buffer.alloc(13);ih.writeUInt32BE(W,0);ih.writeUInt32BE(H,4);ih[8]=8;ih[9]=6;
fs.writeFileSync(process.argv[2],Buffer.concat([Buffer.from([137,80,78,71,13,10,26,10]),chunk('IHDR',ih),chunk('IDAT',zlib.deflateSync(raw)),chunk('IEND',Buffer.alloc(0))]));
