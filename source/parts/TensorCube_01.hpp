typedef CUresult (__stdcall *PFNCUCTXSETCURRENT)(CUcontext);
typedef CUresult (__stdcall *PFNCUMODULELOADDATA)(CUmodule*,const void*);
typedef CUresult (__stdcall *PFNCUMODULEUNLOAD)(CUmodule);
typedef CUresult (__stdcall *PFNCUMODULEGETFUNCTION)(CUfunction*,CUmodule,const char*);
typedef CUresult (__stdcall *PFNCUMEMALLOC)(CUdeviceptr*,unsigned long long);
typedef CUresult (__stdcall *PFNCUMEMFREE)(CUdeviceptr);
typedef CUresult (__stdcall *PFNCUMEMCPYHTOD)(CUdeviceptr,const void*,unsigned long long);
typedef CUresult (__stdcall *PFNCUMEMCPYDTOH)(void*,CUdeviceptr,unsigned long long);
typedef CUresult (__stdcall *PFNCULAUNCHKERNEL)(CUfunction,unsigned int,unsigned int,unsigned int,
    unsigned int,unsigned int,unsigned int,unsigned int,CUstream,void**,void**);
typedef CUresult (__stdcall *PFNCUCTXSYNCHRONIZE)(void);

static HMODULE gCudaDll=0;
static CUdevice gTcDevice=0; static CUcontext gTcCtx=0; static CUmodule gTcModule=0; static CUfunction gTcReduceFn=0;
static PFNCUINIT pcuInit=0; static PFNCUDEVICEGET pcuDeviceGet=0;
static PFNCUDEVICEPRIMARYCTXRETAIN pcuDevicePrimaryCtxRetain=0; static PFNCUDEVICEPRIMARYCTXRELEASE pcuDevicePrimaryCtxRelease=0;
static PFNCUCTXSETCURRENT pcuCtxSetCurrent=0; static PFNCUMODULELOADDATA pcuModuleLoadData=0; static PFNCUMODULEUNLOAD pcuModuleUnload=0;
static PFNCUMODULEGETFUNCTION pcuModuleGetFunction=0; static PFNCUMEMALLOC pcuMemAlloc=0; static PFNCUMEMFREE pcuMemFree=0;
static PFNCUMEMCPYHTOD pcuMemcpyHtoD=0; static PFNCUMEMCPYDTOH pcuMemcpyDtoH=0; static PFNCULAUNCHKERNEL pcuLaunchKernel=0;
static PFNCUCTXSYNCHRONIZE pcuCtxSynchronize=0;
static BOOL gTensorCoreReady=FALSE,gTensorCoreReductionActive=FALSE;
static wchar_t gTensorCoreDiag[256]=L"not initialized";
static CUdeviceptr gTcA=0,gTcB=0,gTcC=0,gTcD=0;
static BYTE* gTcHostA=0; static int* gTcHostD=0;
static unsigned int gTcMatrixCapacity=256u; // enough for current GPU batch result reductions

static const char* kTensorCorePtx=R"PTX(
.version 7.0
.target sm_80
.address_size 64

.visible .entry tc_wmma_u8(
    .param .u64 pA,
    .param .u64 pB,
    .param .u64 pC,
    .param .u64 pD
)
{
    .reg .b32 %r<2>;
    .reg .b64 %rd<10>;
    .reg .b32 %a<2>, %b<2>;
    .reg .s32 %c<8>, %d<8>;

    ld.param.u64 %rd0,[pA];
    ld.param.u64 %rd1,[pB];
    ld.param.u64 %rd2,[pC];
    ld.param.u64 %rd3,[pD];
    mov.u32 %r0,%ctaid.x;
    mul.wide.u32 %rd4,%r0,256;
    mul.wide.u32 %rd5,%r0,1024;
    add.s64 %rd6,%rd0,%rd4;
    add.s64 %rd7,%rd3,%rd5;

    wmma.load.a.sync.aligned.m16n16k16.row.u8 {%a0,%a1},[%rd6];
    wmma.load.b.sync.aligned.m16n16k16.col.u8 {%b0,%b1},[%rd1];
    wmma.load.c.sync.aligned.m16n16k16.row.s32 {%c0,%c1,%c2,%c3,%c4,%c5,%c6,%c7},[%rd2];
    wmma.mma.sync.aligned.m16n16k16.row.col.s32.u8.u8.s32
      {%d0,%d1,%d2,%d3,%d4,%d5,%d6,%d7},
      {%a0,%a1},{%b0,%b1},{%c0,%c1,%c2,%c3,%c4,%c5,%c6,%c7};
    wmma.store.d.sync.aligned.m16n16k16.row.s32
      [%rd7],{%d0,%d1,%d2,%d3,%d4,%d5,%d6,%d7};
    ret;
}
)PTX";

static void setTensorCoreDiag(const wchar_t* t){int i=0;while(t&&*t&&i<255)gTensorCoreDiag[i++]=*t++;gTensorCoreDiag[i]=0;}
static void* tcCudaProc(const char* n){return gCudaDll?GetProcAddress(gCudaDll,n):0;}
static BOOL tcCudaOk(CUresult r){return r==CUDA_SUCCESS?TRUE:FALSE;}
static void shutdownTensorCore(){
    gTensorCoreReady=FALSE;gTensorCoreReductionActive=FALSE;
    if(pcuCtxSetCurrent&&gTcCtx)pcuCtxSetCurrent(gTcCtx);
    if(pcuMemFree){if(gTcA)pcuMemFree(gTcA);if(gTcB)pcuMemFree(gTcB);if(gTcC)pcuMemFree(gTcC);if(gTcD)pcuMemFree(gTcD);}
    gTcA=gTcB=gTcC=gTcD=0;
    if(pcuModuleUnload&&gTcModule)pcuModuleUnload(gTcModule);gTcModule=0;gTcReduceFn=0;
    if(pcuDevicePrimaryCtxRelease&&gTcCtx)pcuDevicePrimaryCtxRelease(gTcDevice);gTcCtx=0;
    vfree(gTcHostA);vfree(gTcHostD);gTcHostA=0;gTcHostD=0;
    if(gCudaDll)FreeLibrary(gCudaDll);gCudaDll=0;
}
static BOOL tensorCoreLaunchMatrices(unsigned int matrices){
    if(!gTensorCoreReady||!matrices||matrices>gTcMatrixCapacity)return FALSE;
    if(!tcCudaOk(pcuCtxSetCurrent(gTcCtx)))return FALSE;
    void* args[4]={&gTcA,&gTcB,&gTcC,&gTcD};
    if(!tcCudaOk(pcuLaunchKernel(gTcReduceFn,matrices,1,1,32,1,1,0,0,args,0)))return FALSE;
    return tcCudaOk(pcuCtxSynchronize());
}
static BOOL initTensorCore(){
    setTensorCoreDiag(L"CUDA driver not available");
    gCudaDll=LoadLibraryW(L"nvcuda.dll");if(!gCudaDll)return FALSE;
#define TCLOAD(dst,name) dst=(decltype(dst))tcCudaProc(name);if(!dst){setTensorCoreDiag(L"CUDA Driver API entry missing");shutdownTensorCore();return FALSE;}
    TCLOAD(pcuInit,"cuInit"); TCLOAD(pcuDeviceGet,"cuDeviceGet");
    TCLOAD(pcuDevicePrimaryCtxRetain,"cuDevicePrimaryCtxRetain");
    pcuDevicePrimaryCtxRelease=(PFNCUDEVICEPRIMARYCTXRELEASE)tcCudaProc("cuDevicePrimaryCtxRelease_v2");
    if(!pcuDevicePrimaryCtxRelease)pcuDevicePrimaryCtxRelease=(PFNCUDEVICEPRIMARYCTXRELEASE)tcCudaProc("cuDevicePrimaryCtxRelease");
    if(!pcuDevicePrimaryCtxRelease){setTensorCoreDiag(L"CUDA Driver API primary-context release missing");shutdownTensorCore();return FALSE;}
    TCLOAD(pcuCtxSetCurrent,"cuCtxSetCurrent"); TCLOAD(pcuModuleLoadData,"cuModuleLoadData");
    TCLOAD(pcuModuleUnload,"cuModuleUnload"); TCLOAD(pcuModuleGetFunction,"cuModuleGetFunction");
    pcuMemAlloc=(PFNCUMEMALLOC)tcCudaProc("cuMemAlloc_v2");if(!pcuMemAlloc)pcuMemAlloc=(PFNCUMEMALLOC)tcCudaProc("cuMemAlloc");
    pcuMemFree=(PFNCUMEMFREE)tcCudaProc("cuMemFree_v2");if(!pcuMemFree)pcuMemFree=(PFNCUMEMFREE)tcCudaProc("cuMemFree");
    pcuMemcpyHtoD=(PFNCUMEMCPYHTOD)tcCudaProc("cuMemcpyHtoD_v2");if(!pcuMemcpyHtoD)pcuMemcpyHtoD=(PFNCUMEMCPYHTOD)tcCudaProc("cuMemcpyHtoD");
    pcuMemcpyDtoH=(PFNCUMEMCPYDTOH)tcCudaProc("cuMemcpyDtoH_v2");if(!pcuMemcpyDtoH)pcuMemcpyDtoH=(PFNCUMEMCPYDTOH)tcCudaProc("cuMemcpyDtoH");
    TCLOAD(pcuLaunchKernel,"cuLaunchKernel"); TCLOAD(pcuCtxSynchronize,"cuCtxSynchronize");
#undef TCLOAD
    if(!pcuMemAlloc||!pcuMemFree||!pcuMemcpyHtoD||!pcuMemcpyDtoH){setTensorCoreDiag(L"CUDA memory API missing");shutdownTensorCore();return FALSE;}
    if(!tcCudaOk(pcuInit(0))||!tcCudaOk(pcuDeviceGet(&gTcDevice,0))||!tcCudaOk(pcuDevicePrimaryCtxRetain(&gTcCtx,gTcDevice))||
       !tcCudaOk(pcuCtxSetCurrent(gTcCtx))||!tcCudaOk(pcuModuleLoadData(&gTcModule,kTensorCorePtx))||
       !tcCudaOk(pcuModuleGetFunction(&gTcReduceFn,gTcModule,"tc_wmma_u8"))){
        setTensorCoreDiag(L"WMMA PTX unavailable on this NVIDIA device/driver");shutdownTensorCore();return FALSE;
    }
    unsigned long long aBytes=(unsigned long long)gTcMatrixCapacity*256ull,dBytes=(unsigned long long)gTcMatrixCapacity*1024ull;
    if(!tcCudaOk(pcuMemAlloc(&gTcA,aBytes))||!tcCudaOk(pcuMemAlloc(&gTcB,256ull))||
       !tcCudaOk(pcuMemAlloc(&gTcC,1024ull))||!tcCudaOk(pcuMemAlloc(&gTcD,dBytes))){
        setTensorCoreDiag(L"Tensor Core buffer allocation failed");shutdownTensorCore();return FALSE;
    }
    gTcHostA=(BYTE*)valloc(aBytes);gTcHostD=(int*)valloc(dBytes);
    if(!gTcHostA||!gTcHostD){setTensorCoreDiag(L"Tensor Core host staging allocation failed");shutdownTensorCore();return FALSE;}
    BYTE A[256],B[256];int C[256],D[256];
    for(int i=0;i<256;i++){A[i]=(BYTE)((i*17+3)&255);B[i]=(BYTE)((i/16)==(i%16)?1:0);C[i]=0;D[i]=0;}
    if(!tcCudaOk(pcuMemcpyHtoD(gTcA,A,256))||!tcCudaOk(pcuMemcpyHtoD(gTcB,B,256))||
       !tcCudaOk(pcuMemcpyHtoD(gTcC,C,1024))){
        setTensorCoreDiag(L"Tensor Core self-test upload failed");shutdownTensorCore();return FALSE;
    }
    gTensorCoreReady=TRUE;
    if(!tensorCoreLaunchMatrices(1)||!tcCudaOk(pcuMemcpyDtoH(D,gTcD,1024))){
        setTensorCoreDiag(L"Tensor Core WMMA self-test launch failed");shutdownTensorCore();return FALSE;
    }
    for(int i=0;i<256;i++)if(D[i]!=(int)A[i]){setTensorCoreDiag(L"Tensor Core WMMA self-test mismatch");shutdownTensorCore();return FALSE;}
    for(int i=0;i<256;i++)B[i]=1;
    if(!tcCudaOk(pcuMemcpyHtoD(gTcB,B,256))||!tcCudaOk(pcuMemcpyHtoD(gTcC,C,1024))){
        setTensorCoreDiag(L"Tensor Core reduction setup failed");shutdownTensorCore();return FALSE;
    }
    gTensorCoreReductionActive=TRUE;setTensorCoreDiag(L"INT8 WMMA exact reduction active");return TRUE;
}
static BOOL tensorCoreReduceGpuResults(const unsigned int* q,unsigned int groups,unsigned long long* tokens,unsigned long long* records,unsigned long long* bad){
    if(!gTensorCoreReady||!gTensorCoreReductionActive||groups<256u)return FALSE;
    unsigned int tiles=(groups+255u)/256u,mats=tiles*6u;if(mats>gTcMatrixCapacity)return FALSE;
    memset(gTcHostA,0,(unsigned long long)mats*256ull);
    for(unsigned int i=0;i<groups;i++){
        unsigned int tile=i>>8,pos=i&255u;
        unsigned int tv=q[i*3u],rv=q[i*3u+1u],bv=q[i*3u+2u];
        gTcHostA[(tile+tiles*0u)*256u+pos]=(BYTE)(tv&255u);
        gTcHostA[(tile+tiles*1u)*256u+pos]=(BYTE)((tv>>8)&255u);
        gTcHostA[(tile+tiles*2u)*256u+pos]=(BYTE)((tv>>16)&255u);
        gTcHostA[(tile+tiles*3u)*256u+pos]=(BYTE)((tv>>24)&255u);
        gTcHostA[(tile+tiles*4u)*256u+pos]=(BYTE)(rv&255u);
