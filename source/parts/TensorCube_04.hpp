static unsigned long long gMacroTokenCount=0;
static unsigned long long gPrimitiveCount=0;
static Cubie gCubies[512];static int gCubieCount=0;

// High-order render state: no materialized cube-face image.
static GLuint gHighRenderProgram=0;
static BOOL gHighRenderReady=FALSE;
static GLint gHrOrder=-1,gHrNonce=-1,gHrKey0=-1,gHrKey1=-1,gHrHasRandom=-1,gHrFace=-1,gHrPlay=-1;
static wchar_t gHighRenderDiag[1024]=L"not initialized";

static GLuint gNearBlurTex=0;
static int gNearBlurW=0,gNearBlurH=0;
static wchar_t gStatusCache[1800]=L"",gGpuCache[1900]=L"",gSolverCache[2300]=L"",gPaperCache[2400]=L"";


static BsgsWorkspace gBsgs;
static SPerm gOrigPerm[TC_MAX_TEMPLATE_GENS];static int gOrigRef[TC_MAX_TEMPLATE_GENS];static unsigned long long gOrigHash[TC_MAX_TEMPLATE_GENS],gOrigInvHash[TC_MAX_TEMPLATE_GENS];
static int gFactorStage=0;
static double gFactorStart=0.0,gFactorElapsedMs=0.0,gLastProgressUi=0.0;
static int gQuotientMoveCount=0;
static int gRandomQuotientBits=-1;

static BOOL gComputeReady=FALSE,gExactPlanPass=FALSE;
static wchar_t gComputeDiag[2048]=L"not initialized",gContextDiag[192]=L"not initialized";
static char gShaderLogBuf[4096];

// Persistent CPU executor pool.  Logical work is not permanently owned by any thread.
enum WorkerJobKind{WK_NONE=0,WK_QUOTIENT=1,WK_FACTOR=2};
struct CpuWorker{HANDLE thread;int index;unsigned int chunkSize;unsigned long long completed,records,tokens;double busySeconds;};
static CpuWorker gCpuWorkers[TC_MAX_CPU_WORKERS];
static HANDLE gCpuWorkerWake[TC_MAX_CPU_WORKERS];
static int gCpuWorkerCount=0;
static volatile LONG gCpuStop=0,gCpuJob=WK_NONE,gCpuWorkersDone=0,gWorkFailed=0;
static volatile LONGLONG gWorkNext=0,gWorkCompleted=0,gCpuCompleted=0,gWorkRecords=0,gWorkTokens=0;
static LONGLONG gWorkTotal=0;

// GPU factorizer slots use asynchronous GL fences; no per-chunk global completion barrier.
struct GpuSlot{GLuint stateBuf,resultBuf;GLsync fence;unsigned int start,count,groups;double submitTime;BOOL busy;};
static GpuSlot gGpuSlots[TC_GPU_SLOTS];
static GLuint gGpuTemplateBuf=0,gGpuGenerateProgram=0,gGpuFactorProgram=0;
static int gGpuTemplateOrder=0;static unsigned int gGpuBatchComponents=TC_GPU_BATCH_START;static BOOL gGpuJobEnabled=FALSE;
static unsigned long long gGpuCompleted=0,gGpuRecords=0,gGpuTokens=0;
static double gGpuBusySeconds=0.0;
static GLint gGenOrder=-1,gGenStart=-1,gGenCount=-1,gGenNonce=-1,gGenKey0=-1,gGenKey1=-1;
static GLint gFacOrder=-1,gFacStart=-1,gFacCount=-1;
static int gGpuCrossCheckState=0;

enum SolveUiState{TC_IDLE=0,TC_RANDOMIZING=1,TC_FACTORIZING=2,TC_PATH_READY=3,TC_PLAYING=4,TC_SOLVED=5,TC_FAILED=6};
static SolveUiState gSolveUiState=TC_IDLE;static wchar_t gSolveFailure[768]=L"";

static int gPlaybackMode=0;
static double gPlaybackValue=1.0,gPlaybackUnit=0.18,gPlaybackStart=0.0,gPlaybackTargetTotal=0.0;
static unsigned long long gPlaybackPrimitiveDone=0;
static int gPlaybackFactorIndex=0,gPlaybackFactorLevel=-1,gPlaybackFactorWordIndex=0,gPlaybackCurrentComp=-1,gPlaybackBsgsType=-1;
static short gPlaybackCurrentToken=0;static BOOL gPlaybackTokenActive=FALSE;
static int gPlaybackSubstep=0,gCurrentMove=-1;static float gCurrentAngle=0.0f;static int gPlaybackWord[10];
static BOOL gPlaybackQuotientApplied=FALSE;

static void render();static void updateStatus();static void updateSolverLine();static void updatePaperData();static void pumpUi();static void setButtons();static BOOL gpuUploadTemplates();
static void mfClearDescriptor();static void mfFreeTemplates();static BOOL mfCreateRandomDescriptor();static BOOL mfStartExactFactorCurrentTensor();static void mfAdvanceExactFactorCurrentTensor();static wchar_t* wappendFixed2(wchar_t*,double);

static void* valloc(unsigned long long bytes){return bytes?VirtualAlloc(0,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE):0;}
static void vfree(void* p){if(p)VirtualFree(p,0,MEM_RELEASE);}
static unsigned long long shellPieceCount(int n){unsigned long long N=(unsigned long long)n;return 6ull*N*N-12ull*N+8ull;}
static int moveCountForOrder(int n){return 9*n;}
static int moveFrom(int axis,int layer,int dir){return ((axis*gOrder+layer)*3)+(dir<0?1:0);}
static void moveInfo(int move,int* axis,int* layer,int* dir,int* turns){int slice=move/3,k=move%3;*axis=slice/gOrder;*layer=slice%gOrder;*dir=(k==1?-1:1);*turns=(k==2?2:1);}
static int inverseMove(int move){int s=move/3,k=move%3;return s*3+(k==0?1:(k==1?0:2));}
static IVec3 negV(IVec3 v){IVec3 r={-v.x,-v.y,-v.z};return r;}
static BOOL ivecEq(IVec3 a,IVec3 b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
static IVec3 dirVec(int code){static const IVec3 d[6]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};return d[code<0?0:(code>5?5:code)];}
static int dirCode(IVec3 v){if(v.x==1)return 0;if(v.x==-1)return 1;if(v.y==1)return 2;if(v.y==-1)return 3;if(v.z==1)return 4;return 5;}
static IVec3 rotateVecQuarter(IVec3 v,int axis,int dir){IVec3 r=v;if(axis==0){if(dir>0){r.y=-v.z;r.z=v.y;}else{r.y=v.z;r.z=-v.y;}}else if(axis==1){if(dir>0){r.x=v.z;r.z=-v.x;}else{r.x=-v.z;r.z=v.x;}}else{if(dir>0){r.x=-v.y;r.y=v.x;}else{r.x=v.y;r.y=-v.x;}}return r;}
static BOOL cubeRotationEq(const CubeRotation& a,const CubeRotation& b){return ivecEq(a.ex,b.ex)&&ivecEq(a.ey,b.ey)&&ivecEq(a.ez,b.ez);}
static IVec3 rotateVecByCubeRotation(IVec3 v,const CubeRotation& r){IVec3 o={r.ex.x*v.x+r.ey.x*v.y+r.ez.x*v.z,r.ex.y*v.x+r.ey.y*v.y+r.ez.y*v.z,r.ex.z*v.x+r.ey.z*v.y+r.ez.z*v.z};return o;}
static void initCubeRotations(){if(gCubeRotationCount==24)return;gCubeRotationCount=1;gCubeRotations[0]={{1,0,0},{0,1,0},{0,0,1}};for(int head=0;head<gCubeRotationCount&&gCubeRotationCount<24;head++){CubeRotation base=gCubeRotations[head];for(int axis=0;axis<3;axis++){CubeRotation r={rotateVecQuarter(base.ex,axis,1),rotateVecQuarter(base.ey,axis,1),rotateVecQuarter(base.ez,axis,1)};BOOL dup=FALSE;for(int i=0;i<gCubeRotationCount;i++)if(cubeRotationEq(gCubeRotations[i],r)){dup=TRUE;break;}if(!dup&&gCubeRotationCount<24)gCubeRotations[gCubeRotationCount++]=r;}}}

static int compTypeFor(int n,int a,int b,int chir){int m=n/2;if(n==2)return 0;if(a==m&&b==m)return 1;if((n&1)&&a==m&&b==0)return 2;if((n&1)&&a==0&&b==0)return 3;if(a==m&&b<m)return 4;if(a==b)return 5;if((n&1)&&b==0)return 6;return 7+chir;}
static int compSizeForType(int t){if(t==0||t==1)return 8;if(t==2)return 12;if(t==3)return 6;return 24;}
static int centeredLevel(int k){return (gOrder&1)?2*k:(2*k-1);}
static BOOL siteLessZYX(IVec3 a,IVec3 b){if(a.z!=b.z)return a.z<b.z;if(a.y!=b.y)return a.y<b.y;return a.x<b.x;}
static int componentSites(const BigComponent& c,IVec3 out[24]){initCubeRotations();int m=gOrder/2,h=gOrder-1;IVec3 v={centeredLevel(m),centeredLevel((int)c.a),c.chir?-centeredLevel((int)c.b):centeredLevel((int)c.b)};int count=0;for(int r=0;r<24;r++){IVec3 q=rotateVecByCubeRotation(v,gCubeRotations[r]);IVec3 p={(q.x+h)/2,(q.y+h)/2,(q.z+h)/2};BOOL dup=FALSE;for(int j=0;j<count;j++)if(ivecEq(out[j],p)){dup=TRUE;break;}if(!dup)out[count++]=p;}for(int i=1;i<count;i++){IVec3 x=out[i];int j=i-1;while(j>=0&&siteLessZYX(x,out[j])){out[j+1]=out[j];j--;}out[j+1]=x;}return count;}
static int findSite(const IVec3* sites,int count,IVec3 p){for(int i=0;i<count;i++)if(ivecEq(sites[i],p))return i;return -1;}
static int findSiteSorted(const IVec3* sites,int count,IVec3 p){int lo=0,hi=count-1;while(lo<=hi){int mid=(lo+hi)>>1;IVec3 q=sites[mid];if(ivecEq(q,p))return mid;BOOL less=siteLessZYX(q,p);if(less)lo=mid+1;else hi=mid-1;}return -1;}
static void applyQuarterToPoint(IVec3* p,IVec3* v,int axis,int layer,int dir){int a[3]={p->x,p->y,p->z};if(a[axis]!=layer)return;int x=p->x,y=p->y,z=p->z;if(axis==0){if(dir>0){p->y=gOrder-1-z;p->z=y;}else{p->y=z;p->z=gOrder-1-y;}}else if(axis==1){if(dir>0){p->x=z;p->z=gOrder-1-x;}else{p->x=gOrder-1-z;p->z=x;}}else{if(dir>0){p->x=gOrder-1-y;p->y=x;}else{p->x=y;p->y=gOrder-1-x;}}*v=rotateVecQuarter(*v,axis,dir);}
static void applyMovePoint(int move,IVec3* p,IVec3* v){int a,l,d,t;moveInfo(move,&a,&l,&d,&t);for(int q=0;q<t;q++)applyQuarterToPoint(p,v,a,l,d);}
static const int kAxisPerm[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
static void macroWordFor(const BigComponent& c,int recipeIndex,int word[10]);
static void generatorPermFor(const BigComponent& c,int recipeIndex,SPerm* out){IVec3 sites[24];int sc=componentSites(c,sites),d=sc*6;int word[10];macroWordFor(c,recipeIndex,word);for(int li=0;li<sc;li++)for(int dc=0;dc<6;dc++){IVec3 p=sites[li],v=dirVec(dc);for(int k=0;k<10;k++)applyMovePoint(word[k],&p,&v);int q=findSite(sites,sc,p);out->p[li*6+dc]=(BYTE)(q*6+dirCode(v));}for(int i=d;i<SG_MAX_DEG;i++)out->p[i]=(BYTE)i;}
static BOOL primitivePermForComponent(const BigComponent& c,int move,SPerm* out){IVec3 sites[24];int sc=componentSites(c,sites),d=sc*6;for(int li=0;li<sc;li++)for(int dc=0;dc<6;dc++){IVec3 p=sites[li],v=dirVec(dc);applyMovePoint(move,&p,&v);int q=findSiteSorted(sites,sc,p);if(q<0)return FALSE;out->p[li*6+dc]=(BYTE)(q*6+dirCode(v));}for(int i=d;i<SG_MAX_DEG;i++)out->p[i]=(BYTE)i;return TRUE;}

