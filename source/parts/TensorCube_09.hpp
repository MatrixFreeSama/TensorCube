const int ROTLOCAL[216]=int[216](7,5,3,6,1,1,7,2,2,4,4,3,0,3,0,5,6,6,0,5,2,7,1,4,7,5,3,6,1,1,7,2,2,4,4,3,0,3,0,5,6,6,0,5,2,7,1,4,7,10,3,6,5,2,11,6,1,9,4,2,0,7,1,8,9,11,4,5,3,10,0,8,3,3,0,4,3,0,4,2,0,5,2,3,0,4,2,5,2,5,1,1,4,5,1,1,15,19,7,14,9,3,23,10,4,18,12,5,0,11,2,17,20,22,8,13,6,21,1,16,17,15,3,18,7,1,19,8,2,20,14,9,0,11,6,21,16,22,4,13,10,23,5,12,13,18,3,14,11,2,19,12,1,21,10,6,0,15,5,20,17,23,8,9,7,22,4,16,14,17,3,15,9,1,19,10,2,21,13,6,0,11,5,20,18,23,8,12,7,22,4,16,10,18,3,11,13,2,19,14,1,22,9,5,0,15,6,20,17,23,12,8,7,21,4,16);
const int LOCALROT[216]=int[216](12,4,7,2,9,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,4,7,2,9,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,8,5,2,10,4,3,0,15,9,1,6,0,0,0,0,0,0,0,0,0,0,0,0,2,18,7,0,3,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,22,14,5,8,11,20,2,18,4,7,13,10,19,3,0,23,15,9,1,16,21,17,6,12,5,8,2,18,22,14,4,7,11,20,13,23,19,10,1,16,0,3,6,9,15,17,21,12,8,5,2,22,14,11,20,18,19,10,4,7,0,3,13,23,16,1,6,15,9,21,17,12,5,8,2,22,14,11,20,18,4,7,13,19,10,0,3,23,1,16,6,15,9,21,17,12,8,5,2,22,11,14,20,19,10,0,3,18,4,7,13,23,16,1,6,15,21,9,17);
uint rotl32(uint x,uint n){return (x<<n)|(x>>(32u-n));}
void qr(inout uint a,inout uint b,inout uint c,inout uint d){a+=b;d^=a;d=rotl32(d,16u);c+=d;b^=c;b=rotl32(b,12u);a+=b;d^=a;d=rotl32(d,8u);c+=d;b^=c;b=rotl32(b,7u);}
uint kw(uint i){return i<4u?key0[i]:key1[i-4u];}
uint rand32(uint id,uint lane,uint domain){uint ss[16],xx[16];ss[0]=0x61707865u;ss[1]=0x3320646eu;ss[2]=0x79622d32u;ss[3]=0x6b206574u;for(uint i=0u;i<8u;i++)ss[4u+i]=kw(i);ss[12]=id;ss[13]=domain^(lane*0x9E3779B9u);ss[14]=0u;ss[15]=nonce;for(uint i=0u;i<16u;i++)xx[i]=ss[i];for(int r=0;r<10;r++){qr(xx[0],xx[4],xx[8],xx[12]);qr(xx[1],xx[5],xx[9],xx[13]);qr(xx[2],xx[6],xx[10],xx[14]);qr(xx[3],xx[7],xx[11],xx[15]);qr(xx[0],xx[5],xx[10],xx[15]);qr(xx[1],xx[6],xx[11],xx[12]);qr(xx[2],xx[7],xx[8],xx[13]);qr(xx[3],xx[4],xx[9],xx[14]);}uint j=lane&15u;return xx[j]+ss[j];}
uint boundv(uint id,uint lane,uint domain,uint b){if(b<=1u)return 0u;uint th=(0u-b)%b;for(uint a=0u;;a++){uint r=rand32(id,lane+a*17u,domain);if(r>=th)return r%b;}}
uint qbit(uint cls){return rand32(cls,0u,0x514F544Du)&1u;}
uint typeOf(uint n,uint a,uint b,uint ch){uint m=n/2u;if(n==2u)return 0u;if(a==m&&b==m)return 1u;if((n&1u)!=0u&&a==m&&b==0u)return 2u;if((n&1u)!=0u&&a==0u&&b==0u)return 3u;if(a==m&&b<m)return 4u;if(a==b)return 5u;if((n&1u)!=0u&&b==0u)return 6u;return 7u+ch;}
uint sizeOf(uint ty){if(ty==0u||ty==1u)return 8u;if(ty==2u)return 12u;if(ty==3u)return 6u;return 24u;}
int centered(uint k){return (orderN&1u)!=0u?int(2u*k):int(2u*k)-1;}
ivec3 rotq(ivec3 v,int r){return REX[r]*v.x+REY[r]*v.y+REZ[r]*v.z;}
ivec3 canon(uint a,uint b,uint ch){uint m=orderN/2u;return ivec3(centered(m),centered(a),ch!=0u?-centered(b):centered(b));}
int localFor(uint ty,uint a,uint b,uint ch,ivec3 q){ivec3 v=canon(a,b,ch);for(int r=0;r<24;r++)if(all(equal(rotq(v,r),q)))return ROTLOCAL[int(ty)*24+r];return -1;}
ivec3 siteForLocal(uint ty,uint a,uint b,uint ch,int li){int r=LOCALROT[int(ty)*24+li];ivec3 q=rotq(canon(a,b,ch),r);int h=int(orderN)-1;return (q+ivec3(h))/2;}
uint ordinal(uint a,uint b,uint ch){uint m=orderN/2u;bool odd=(orderN&1u)!=0u;uint top=odd?m+1u:m;if(a==m)return m-b;if(odd&&a==0u&&b==0u)return top+m*(m-1u);uint st=odd?(top+m*(m-1u)-a*(a+1u)):(top+(m-1u)*(m-1u)-a*a);uint off;if(b==a)off=0u;else if(odd&&b==0u)off=2u*a-1u;else off=1u+2u*(a-1u-b)+ch;return st+off;}
uint levelAbs(int v){uint x=uint(v<0?-v:v);return (orderN&1u)!=0u?x/2u:(x+1u)/2u;}
bool componentForSite(ivec3 p,out uint a,out uint b,out uint ch,out uint ty,out uint sz,out uint ci,out int li){int h=int(orderN)-1;ivec3 q=2*p-ivec3(h);int av0=abs(q.x),av1=abs(q.y),av2=abs(q.z),tmp;if(av1>av0){tmp=av0;av0=av1;av1=tmp;}if(av2>av1){tmp=av1;av1=av2;av2=tmp;}if(av1>av0){tmp=av0;av0=av1;av1=tmp;}if(av0!=h)return false;a=levelAbs(av1);b=levelAbs(av2);uint m=orderN/2u;int maxch=(m>a&&a>b&&b>0u)?2:1;for(int c=0;c<maxch;c++){uint tty=typeOf(orderN,a,b,uint(c));int l=localFor(tty,a,b,uint(c),q);if(l>=0){ch=uint(c);ty=tty;sz=sizeOf(ty);ci=ordinal(a,b,ch);li=l;return true;}}return false;}
uint cc(uint ty,uint lev){return t[OFF_CC+ty*LEVELS+lev];}
uint choice(uint ty,uint lev,uint k){return t[OFF_CHOICES+(ty*LEVELS+lev)*DEG+k];}
uint trb(uint off,uint ty,uint lev,uint beta,uint i){uint q=off+(((ty*LEVELS+lev)*DEG+beta)*WORDS)+(i>>2);return (t[q]>>((i&3u)*8u))&255u;}
uint kernelPreimage(uint id,uint ty,uint target){if(ty==3u)return target;uint cur=target,bl=t[OFF_BL+ty];for(uint lev=0u;lev<bl;lev++){uint n=cc(ty,lev);if(n==0u)break;uint k=boundv(id,lev,0x4B45524Eu^ty,n);uint beta=choice(ty,lev,k);cur=trb(OFF_INV,ty,lev,beta,cur);}return cur;}
ivec3 dvec(int c){if(c==0)return ivec3(1,0,0);if(c==1)return ivec3(-1,0,0);if(c==2)return ivec3(0,1,0);if(c==3)return ivec3(0,-1,0);if(c==4)return ivec3(0,0,1);return ivec3(0,0,-1);}
int dcode(ivec3 v){if(v.x==1)return 0;if(v.x==-1)return 1;if(v.y==1)return 2;if(v.y==-1)return 3;if(v.z==1)return 4;return 5;}
void qxInv(inout ivec3 p,inout ivec3 v,int layer){if(p.x!=layer)return;int y=p.y,z=p.z,n=int(orderN)-1;p.y=z;p.z=n-y;int vy=v.y,vz=v.z;v.y=vz;v.z=-vy;}
void undoGeneratedQ(uint a,uint b,inout ivec3 p,inout ivec3 v){uint m=orderN/2u;uint lv[3]=uint[3](m,a,b);for(int k=2;k>=0;k--){bool dup=false;for(int j=0;j<k;j++)if(lv[j]==lv[k])dup=true;if(dup)continue;uint cls=m-lv[k];if(qbit(cls)!=0u)qxInv(p,v,int(cls));}}
bool stickerAt(ivec3 p,int f){int n=int(orderN)-1;if(f==0)return p.x==n;if(f==1)return p.x==0;if(f==2)return p.y==n;if(f==3)return p.y==0;if(f==4)return p.z==n;return p.z==0;}
vec3 pal(int f){if(f==0)return vec3(0.902,0.078,0.059);if(f==1)return vec3(1.0,0.38,0.031);if(f==2)return vec3(0.96);if(f==3)return vec3(0.98,0.82,0.051);if(f==4)return vec3(0.051,0.72,0.22);return vec3(0.039,0.278,0.949);}
uint hashSite(ivec3 p,int f){uint h=uint(p.x)*73856093u^uint(p.y)*19349663u^uint(p.z)*83492791u^uint(f)*2654435761u;h^=h>>16;h*=2246822519u;h^=h>>13;return h;}
vec3 stateColor(ivec3 p,int f){if(hasRandom==0)return pal(f);uint a,b,ch,ty,sz,ci;int li;if(!componentForSite(p,a,b,ch,ty,sz,ci,li))return pal(f);ivec3 tp=p,tv=dvec(f);undoGeneratedQ(a,b,tp,tv);int tli=localFor(ty,a,b,ch,2*tp-ivec3(int(orderN)-1));if(tli<0)return pal(f);uint target=uint(tli*6+dcode(tv));uint pre=kernelPreimage(ci,ty,target);int sli=int(pre/6u),sf=int(pre%6u);ivec3 sp=siteForLocal(ty,a,b,ch,sli);if(!stickerAt(sp,sf))return pal(f);vec3 c=pal(sf);if(playP>0.0){float th=float(hashSite(p,f)&65535u)/65535.0;if(playP>th)c=pal(f);else if(abs(playP-th)<0.018)c=min(vec3(1.0),c*1.18+vec3(0.06));}return c;}
void main(){float h=2.856;ivec3 p=ivec3(0);vec2 uv=vec2(0);float N=float(orderN);if(faceId==0||faceId==1){uv=clamp(vec2((vLocal.z/h+1.0)*0.5,(vLocal.y/h+1.0)*0.5),0.0,0.999999);p=ivec3(faceId==0?int(orderN)-1:0,int(floor(uv.y*N)),int(floor(uv.x*N)));}else if(faceId==2||faceId==3){uv=clamp(vec2((vLocal.x/h+1.0)*0.5,(vLocal.z/h+1.0)*0.5),0.0,0.999999);p=ivec3(int(floor(uv.x*N)),faceId==2?int(orderN)-1:0,int(floor(uv.y*N)));}else{uv=clamp(vec2((vLocal.x/h+1.0)*0.5,(vLocal.y/h+1.0)*0.5),0.0,0.999999);p=ivec3(int(floor(uv.x*N)),int(floor(uv.y*N)),faceId==4?int(orderN)-1:0);}vec3 c=stateColor(p,faceId);float footprint=max(fwidth(uv.x),fwidth(uv.y))*N;if(footprint<1.6){vec2 g=fract(uv*N);float e=min(min(g.x,1.0-g.x),min(g.y,1.0-g.y));float w=max(0.018,0.045*footprint);if(e<w)c*=0.12;}gl_FragColor=vec4(c,1.0);}
)GLSL";

static BOOL initHighMatrixFreeRenderer(){
    gHighRenderProgram=compileRenderProgram(kHighRenderVertex,kHighRenderFragment);
    if(!gHighRenderProgram){gHighRenderReady=FALSE;return FALSE;}
    gHrOrder=pglGetUniformLocation(gHighRenderProgram,"orderN");
    gHrNonce=pglGetUniformLocation(gHighRenderProgram,"nonce");
    gHrKey0=pglGetUniformLocation(gHighRenderProgram,"key0");
    gHrKey1=pglGetUniformLocation(gHighRenderProgram,"key1");
    gHrHasRandom=pglGetUniformLocation(gHighRenderProgram,"hasRandom");
    gHrFace=pglGetUniformLocation(gHighRenderProgram,"faceId");
    gHrPlay=pglGetUniformLocation(gHighRenderProgram,"playP");
    gHighRenderReady=TRUE;setDiag(gHighRenderDiag,1024,L"direct fragment Matrix-Free renderer initialized");return TRUE;
}

static BOOL initComputePipeline(){
    setDiag(gComputeDiag,2048,L"initializing resource-return GPU factorizer");
    pglCreateShader=(PFNGLCREATESHADERPROC)loadGL("glCreateShader");if(!requireComputeEntry((void*)pglCreateShader,"glCreateShader"))return FALSE;
    pglShaderSource=(PFNGLSHADERSOURCEPROC)loadGL("glShaderSource");if(!requireComputeEntry((void*)pglShaderSource,"glShaderSource"))return FALSE;
    pglCompileShader=(PFNGLCOMPILESHADERPROC)loadGL("glCompileShader");if(!requireComputeEntry((void*)pglCompileShader,"glCompileShader"))return FALSE;
    pglGetShaderiv=(PFNGLGETSHADERIVPROC)loadGL("glGetShaderiv");if(!requireComputeEntry((void*)pglGetShaderiv,"glGetShaderiv"))return FALSE;
