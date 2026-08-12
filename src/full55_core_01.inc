static u32 f55Perm[F55_MAX_ORBIT];
static u8  f55Visited[F55_MAX_ORBIT];
static u32 f55VerifyIdAt[F55_MAX_ORBIT];
static u32 f55VerifySiteOf[F55_MAX_ORBIT];
static u64 f55VerifyOri[F55_MAX_ORBIT];
static u32 f55Plan[F55_MAX_PLAN];
static u32 f55PlanCount=0;
static u32 f55PlanStart[12];
static u32 f55PlanLen[12];
static u64 f55PlanPlaneMask=0;
static u64 f55ScramblePlaneMask=0;

// Small public geometry lookup tables.
static F55_Opt f55PairOpt[4][64][64][3]; static u8 f55PairOptN[4][64][64];
static u16 f55CycByVert[4][64][13]; static u8 f55CycByVertN[4][64];
static F55_Axes3 f55Triples[165]; static u16 f55TripleMask[165]; static int f55TripleN=0;
static u16 f55TriplesByMask[2048][45]; static u8 f55TriplesByMaskN[2048];
static int f55TablesReady=0;

static int f55Code3(int a,int b,int c){return a|(b<<2)|(c<<4);} 
static void f55Dec3(int x,int* a,int* b,int* c){*a=x&3;*b=(x>>2)&3;*c=(x>>4)&3;}
static int f55Bc3(int x){int a,b,c;f55Dec3(x,&a,&b,&c);return (a==0||a==3)+(b==0||b==3)+(c==0||c==3);} 
static int f55Get(u32 x,int a){return (int)((x>>(2*a))&3u);} 
static u32 f55Set(u32 x,int a,int v){u32 m=3u<<(2*a);return (x&~m)|((u32)v<<(2*a));}
static int f55Bcount(u32 x){int z=0;for(int a=0;a<F55_D;a++){int v=f55Get(x,a);z+=(v==0||v==3);}return z;}
static int f55Pop(u64 x){int n=0;while(x){x&=x-1;n++;}return n;}
static int f55PlaneIndex(int a,int b){if(a>b){int t=a;a=b;b=t;}int q=0;for(int i=0;i<F55_D;i++)for(int j=i+1;j<F55_D;j++,q++)if(i==a&&j==b)return q;return -1;}

// Packed signed 11-axis orientation: 5 bits per source axis, low4=dest, bit4=negative.
static u64 f55OriId(){u64 z=0;for(int i=0;i<F55_D;i++)z|=(u64)i<<(5*i);return z;}
static int f55OriField(u64 o,int src){int f=(int)((o>>(5*src))&31ull),ax=f&15;return (f&16)?-(ax+1):(ax+1);}
static u64 f55OriSet(u64 o,int src,int signedAx){int sg=signedAx<0,ax=(signedAx<0?-signedAx:signedAx)-1;u64 m=31ull<<(5*src),f=(u64)(ax|(sg?16:0));return (o&~m)|(f<<(5*src));}
static int f55OriMapSigned(u64 o,int s){int sg=s<0?-1:1,ax=(s<0?-s:s)-1;return sg*f55OriField(o,ax);}
static u64 f55OriCompose(u64 A,u64 B){u64 z=0;for(int i=0;i<F55_D;i++)z=f55OriSet(z,i,f55OriMapSigned(A,f55OriField(B,i)));return z;}
static u64 f55OriInv(u64 A){u64 z=0;for(int i=0;i<F55_D;i++){int v=f55OriField(A,i),sg=v<0?-1:1,j=(v<0?-v:v)-1;z=f55OriSet(z,j,sg*(i+1));}return z;}
static int f55OriIsId(u64 o){return o==f55OriId();}
static int f55InverseNormalAxis(u64 ori,int worldAxis,int worldSign,int* srcAxis,int* srcSign){for(int s=0;s<F55_D;s++){int v=f55OriField(ori,s),sg=v<0?-1:1,d=(v<0?-v:v)-1;if(d==worldAxis){*srcAxis=s;*srcSign=worldSign*sg;return 1;}}return 0;}

static int f55RotLocalAxis(int fixed,int dir,int localAxis){int sg=1,out=localAxis;if(fixed==0){if(dir>0){if(localAxis==1)out=2;else if(localAxis==2){out=1;sg=-1;}}else{if(localAxis==1){out=2;sg=-1;}else if(localAxis==2)out=1;}}else if(fixed==1){if(dir>0){if(localAxis==0){out=2;sg=-1;}else if(localAxis==2)out=0;}else{if(localAxis==0)out=2;else if(localAxis==2){out=0;sg=-1;}}}else{if(dir>0){if(localAxis==0)out=1;else if(localAxis==1){out=0;sg=-1;}}else{if(localAxis==0){out=1;sg=-1;}else if(localAxis==1)out=0;}}return sg*(out+1);}
static int f55RotateWorldSigned(int s,const F55_Axes3*axes,int fixed,int dir){int sg=s<0?-1:1,ax=(s<0?-s:s)-1,l=-1;for(int i=0;i<3;i++)if((int)axes->a[i]==ax)l=i;if(l<0)return s;int q=f55RotLocalAxis(fixed,dir,l),qsg=q<0?-1:1,ql=(q<0?-q:q)-1;return sg*qsg*((int)axes->a[ql]+1);}
