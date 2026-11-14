import numpy as np
from PIL import Image
rng=np.random.default_rng(5453)
def noise1d(n,scale,amp):
    v=rng.normal(0,1,n//scale+3); x=np.arange(n)/scale
    return amp*np.interp(x,np.arange(len(v)),v)
h=np.array(Image.open('Art/Source/level-end-screen/20261029-174247-level-end-screen-1-keyed.png')).astype(np.float32)
im=h[26:68,128:896].copy(); H,W=im.shape[:2]
# mute the teal line toward target (68,180,171)
for y in range(0,9):
    var=1+0.18*noise1d(W,30,1.0)[:,None]
    im[y,:,:3]=np.clip(im[y,:,:3]*0.45*var+np.array([38,120,112])*0.30*(im[y,:,:3].mean(1,keepdims=True)/120),0,255)
body=np.array([74,95,76],np.float32)
for y in range(9,H):
    L=im[y,:,:3].mean(1,keepdims=True); im[y,:,:3]=np.clip(L/58.0*body,0,255)
for y,k in ((H-8,0.55),(H-7,0.35)):
    im[y,:,:3]=im[y,:,:3]*(1-k)+np.array([118,136,112])*k
# rust runs: 1D noise along each long edge -> depth profile, hard threshold
def noise1d(n,scale,amp):
    v=rng.normal(0,1,n//scale+3); x=np.arange(n)/scale
    return amp*np.interp(x,np.arange(len(v)),v)
def runs(edge_top):
    base=noise1d(W,40,1.0)+noise1d(W,9,0.45)
    depth=np.clip((base-0.25)*3,0,2)*(noise1d(W,3,1.0)>-0.3)             # px of rust into the bar, 0 where clean
    pass  # bar ends
    tone=noise1d(W,6,1.0)
    for x in range(W):
        d=depth[x]
        if d<=0: continue
        for i in range(int(d)+1):
            y = (9+i) if edge_top else H-9-i
            if im[y,x,3]<128: continue
            f=i/(d+1e-3)
            dark=np.array([70,56,40]); orange=np.array([112,82,54])
            t=np.clip(0.5+0.35*tone[x]-0.3*f,0,1)
            col=dark*(1-t)+orange*t
            k=0.7*(1-f)**0.8+0.1          # short stain bleeding onto the face
            im[y,x,:3]=im[y,x,:3]*(1-k)+col*k
runs(True); runs(False)
out=np.clip(im,0,255).astype(np.uint8)
Image.fromarray(out).save('Art/Source/ui-frame-pieces/T_UI_BarH.png')
Image.fromarray(out).rotate(90,expand=True).save('Art/Source/ui-frame-pieces/T_UI_BarV.png')
bg=Image.new('RGBA',(820,80),(10,12,14,255)); bg.alpha_composite(Image.fromarray(out),(20,18))
bg.crop((0,0,410,80)).resize((1230,240),Image.LANCZOS).convert('RGB').save('Saved/Scratch/task545/bars_preview3.png')
