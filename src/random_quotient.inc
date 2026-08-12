// Source-v32 ChaCha20 block construction, retained exactly at the primitive
// level; only the consuming domain is the shuffle stream.
static u32 rotl32(u32 x,int n){return (x<<n)|(x>>(32-n));}
static u32 load32le(const BYTE*p){return (u32)p[0]|((u32)p[1]<<8)|((u32)p[2]<<16)|((u32)p[3]<<24);}
static void store32le(BYTE*p,u32 v){p[0]=(BYTE)v;p[1]=(BYTE)(v>>8);p[2]=(BYTE)(v>>16);p[3]=(BYTE)(v>>24);}
static void chachaQr(u32&a,u32&b,u32&c,u32&d){a+=b;d^=a;d=rotl32(d,16);c+=d;b^=c;b=rotl32(b,12);a+=b;d^=a;d=rotl32(d,8);c+=d;b^=c;b=rotl32(b,7);}
struct DeepRandom{
    BYTE key[32];u32 nonce;u64 blockIndex;u32 lane;BYTE block[64];
    void init(const BYTE seed[36]){for(int i=0;i<32;i++)key[i]=seed[i];nonce=load32le(seed+32);blockIndex=0;lane=16;secureZero(block,sizeof(block));}
    void refill(){static const u32 C[4]={0x61707865u,0x3320646eu,0x79622d32u,0x6b206574u};u32 st[16],x[16];st[0]=C[0];st[1]=C[1];st[2]=C[2];st[3]=C[3];for(int i=0;i<8;i++)st[4+i]=load32le(key+i*4);st[12]=(u32)blockIndex;st[13]=0x46353544u;st[14]=(u32)(blockIndex>>32);st[15]=nonce;for(int i=0;i<16;i++)x[i]=st[i];for(int r=0;r<10;r++){chachaQr(x[0],x[4],x[8],x[12]);chachaQr(x[1],x[5],x[9],x[13]);chachaQr(x[2],x[6],x[10],x[14]);chachaQr(x[3],x[7],x[11],x[15]);chachaQr(x[0],x[5],x[10],x[15]);chachaQr(x[1],x[6],x[11],x[12]);chachaQr(x[2],x[7],x[8],x[13]);chachaQr(x[3],x[4],x[9],x[14]);}for(int i=0;i<16;i++){x[i]+=st[i];store32le(block+4*i,x[i]);}blockIndex++;lane=0;secureZero(x,sizeof(x));secureZero(st,sizeof(st));}
    u32 next32(){if(lane>=16)refill();return load32le(block+4*(lane++));}
    u32 bound(u32 b){if(b<=1u)return 0u;u32 threshold=(0u-b)%b;for(;;){u32 r=next32();if(r>=threshold)return r%b;}}
    void clear(){secureZero(this,sizeof(*this));}
};
static u32 deepRandomBound(void*ctx,u32 b){return ((DeepRandom*)ctx)->bound(b);}

static u64 f55QuotientRot(int a,int b,int dir){u64 o=f55OriId();if(dir>0){o=f55OriSet(o,a,b+1);o=f55OriSet(o,b,-(a+1));}else{o=f55OriSet(o,a,-(b+1));o=f55OriSet(o,b,a+1);}return o;}
// Canonical quotient generator m rotates the (X10,X11) plane while the other
// nine axes are fixed with exactly m boundary coordinates.  On the tracked
// surface its parity vector is e_m + e_(m+2), omitting the interior e_0 term.
static u16 f55QuotientVector(int m){u16 z=0;if(m>=1)z|=(u16)(1u<<(m-1));if(m+2<=11)z|=(u16)(1u<<(m+1));return z;}
static void f55ApplyQuotientMove(int m,int dir){const int A=9,B=10;u32 pos[16],oldId[16];for(int x=0;x<4;x++)for(int y=0;y<4;y++){u32 site=0;for(int ax=0;ax<9;ax++)site=f55Set(site,ax,ax<m?0:1);site=f55Set(site,A,x);site=f55Set(site,B,y);pos[x*4+y]=site;oldId[x*4+y]=f55IdAtSite[site];}u64 R=f55QuotientRot(A,B,dir);for(int x=0;x<4;x++)for(int y=0;y<4;y++){int nx,ny;if(dir>0){nx=3-y;ny=x;}else{nx=y;ny=3-x;}u32 dst=pos[nx*4+ny],id=oldId[x*4+y];f55IdAtSite[dst]=id;f55SiteOfId[id]=dst;f55OriOfId[id]=f55OriCompose(R,f55OriOfId[id]);}}
static void f55ApplyQuotientMask(u16 qmask,BOOL inverse){if(inverse){for(int m=9;m>=0;m--)if(qmask&(1u<<m))f55ApplyQuotientMove(m,-1);}else{for(int m=0;m<10;m++)if(qmask&(1u<<m))f55ApplyQuotientMove(m,+1);}}
static int f55OrbitParity(int k){F55_Orbit O={k,F55_ORBIT_OFF[k],F55_ORBIT_N[k],f55OrbitMask[k]};for(u32 i=0;i<O.n;i++){u32 gid=f55BasisSite[O.off+i],cur=f55SiteOfId[gid],gr=f55BasisRankBySite[cur];if(gr==F55_BAD||gr<O.off||gr>=O.off+O.n)return -1;f55Perm[i]=gr-O.off;}for(u32 i=0;i<O.n;i++)f55Visited[i]=0;u32 cycles=0;for(u32 i=0;i<O.n;i++)if(!f55Visited[i]){cycles++;u32 j=i;while(!f55Visited[j]){f55Visited[j]=1;j=f55Perm[j];if(j>=O.n)return -1;}}return (int)((O.n-cycles)&1u);}
static BOOL f55ObservedParityMask(u16*out){u16 z=0;for(int k=1;k<=11;k++){int q=f55OrbitParity(k);if(q<0)return FALSE;if(q)z|=(u16)(1u<<(k-1));}*out=z;return TRUE;}
static BOOL f55RecoverQuotientFromParity(u16 pm,u16*out){u16 row[11];u8 rhs[11];int piv[10];for(int r=0;r<11;r++){u16 c=0;for(int j=0;j<10;j++)if(f55QuotientVector(j)&(1u<<r))c|=(u16)(1u<<j);row[r]=c;rhs[r]=(u8)((pm>>r)&1u);}int prow=0;for(int c=0;c<10;c++){int r=prow;while(r<11&&!(row[r]&(1u<<c)))r++;if(r==11){piv[c]=-1;continue;}u16 tr=row[prow];row[prow]=row[r];row[r]=tr;u8 tv=rhs[prow];rhs[prow]=rhs[r];rhs[r]=tv;piv[c]=prow;for(int rr=0;rr<11;rr++)if(rr!=prow&&(row[rr]&(1u<<c))){row[rr]^=row[prow];rhs[rr]^=rhs[prow];}prow++;}for(int r=0;r<11;r++)if(row[r]==0&&rhs[r])return FALSE;u16 x=0;for(int c=0;c<10;c++)if(piv[c]>=0&&rhs[piv[c]])x|=(u16)(1u<<c);*out=x;return TRUE;}
static BOOL f55RecoverQuotientFromState(u16*out){u16 pm=0;if(!f55ObservedParityMask(&pm))return FALSE;return f55RecoverQuotientFromParity(pm,out);}
static int f55QuotientCount(u16 q){int n=0;while(q){q&=(u16)(q-1);n++;}return n;}
static void f55ApplyStarPlanInverseGlobal(){for(u32 z=f55PlanCount;z>0;z--){u32 q=f55Plan[z-1],v=q&0xfffffu,sg=(q&(1u<<20))?-1:1,k=(q>>21)&15u;F55_Orbit O={(int)k,F55_ORBIT_OFF[k],F55_ORBIT_N[k],f55OrbitMask[k]};f55ApplyStarGlobal(&O,v,-sg);}}
static BOOL f55QuotientAlgebraSelfTest(){for(u16 q=0;q<1024;q++){u16 pm=0;for(int m=0;m<10;m++)if(q&(1u<<m))pm^=f55QuotientVector(m);u16 r=0;if(!f55RecoverQuotientFromParity(pm,&r)||r!=q)return FALSE;}return TRUE;}

static HANDLE gScrambleThread=0;
static volatile BOOL gScrambleWorking=0,gScrambleOk=0;
static double gBasisBuildSeconds=0.0,gLastScrambleSeconds=0.0;
static u64 gLastScrambleOps=0;
static u32 gPlaybackApplied=0;
static u16 gRecoveredQuotientMask=0;
static int gRecoveredQuotientCount=0;
static int gPlaybackQuotientApplied=0;
static int gLastScrambleQuotientCount=0;

static DWORD __stdcall scrambleThread(void*){
    BOOL ok=TRUE;double b0=nowSeconds();
    if(!f55BasisReady)ok=f55BuildBasis()?TRUE:FALSE;
    double b1=nowSeconds();if(gBasisBuildSeconds<=0.0)gBasisBuildSeconds=b1-b0;
    BYTE seed[36];secureZero(seed,sizeof(seed));DeepRandom rng;secureZero(&rng,sizeof(rng));
    double s0=nowSeconds();u64 ops=0;u16 generatedQ=0,recoveredQ=0;int qcount=0;
    if(ok){if(BCryptGenRandom(0,seed,36,BCRYPT_USE_SYSTEM_PREFERRED_RNG)!=0)ok=FALSE;else{
        rng.init(seed);secureZero(seed,sizeof(seed));
        ok=f55Scramble(deepRandomBound,&rng,&ops)?TRUE:FALSE;
        if(ok){for(int m=0;m<10;m++)if(rng.bound(2)){generatedQ|=(u16)(1u<<m);qcount++;}f55ApplyQuotientMask(generatedQ,FALSE);if(!f55RecoverQuotientFromState(&recoveredQ)||recoveredQ!=generatedQ)ok=FALSE;}
        rng.clear();
    }}
    secureZero(seed,sizeof(seed));
    gLastScrambleSeconds=nowSeconds()-s0;gLastScrambleOps=ops+(u64)qcount;gLastScrambleQuotientCount=qcount;
    if(ok&&f55Pop(f55ScramblePlaneMask)!=55)ok=FALSE;
    secureZero(&generatedQ,sizeof(generatedQ));secureZero(&recoveredQ,sizeof(recoveredQ));
    gScrambleOk=ok;gScrambleWorking=FALSE;return 0;
}
