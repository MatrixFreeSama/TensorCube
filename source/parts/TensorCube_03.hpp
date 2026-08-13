static const int WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;

// Extension function types
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef long long GLsizeiptr;
typedef char GLchar;
typedef unsigned char GLboolean;
typedef HGLRC (__stdcall *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);
typedef GLuint (__stdcall *PFNGLCREATESHADERPROC)(unsigned int);
typedef void (__stdcall *PFNGLSHADERSOURCEPROC)(GLuint,GLsizei,const GLchar* const*,const GLint*);
typedef void (__stdcall *PFNGLCOMPILESHADERPROC)(GLuint);
typedef void (__stdcall *PFNGLGETSHADERIVPROC)(GLuint,unsigned int,GLint*);
typedef void (__stdcall *PFNGLGETSHADERINFOLOGPROC)(GLuint,GLsizei,GLsizei*,GLchar*);
typedef GLuint (__stdcall *PFNGLCREATEPROGRAMPROC)(void);
typedef void (__stdcall *PFNGLATTACHSHADERPROC)(GLuint,GLuint);
typedef void (__stdcall *PFNGLLINKPROGRAMPROC)(GLuint);
typedef void (__stdcall *PFNGLGETPROGRAMIVPROC)(GLuint,unsigned int,GLint*);
typedef void (__stdcall *PFNGLGETPROGRAMINFOLOGPROC)(GLuint,GLsizei,GLsizei*,GLchar*);
typedef void (__stdcall *PFNGLUSEPROGRAMPROC)(GLuint);
typedef GLint (__stdcall *PFNGLGETUNIFORMLOCATIONPROC)(GLuint,const GLchar*);
typedef void (__stdcall *PFNGLUNIFORM1IPROC)(GLint,GLint);
typedef void (__stdcall *PFNGLUNIFORM1UIPROC)(GLint,unsigned int);
typedef void (__stdcall *PFNGLUNIFORM1FPROC)(GLint,float);
typedef void (__stdcall *PFNGLUNIFORM4UIPROC)(GLint,unsigned int,unsigned int,unsigned int,unsigned int);
typedef void (__stdcall *PFNGLGENBUFFERSPROC)(GLsizei,GLuint*);
typedef void (__stdcall *PFNGLBINDBUFFERPROC)(unsigned int,GLuint);
typedef void (__stdcall *PFNGLBUFFERDATAPROC)(unsigned int,GLsizeiptr,const void*,unsigned int);
typedef void (__stdcall *PFNGLBINDBUFFERBASEPROC)(unsigned int,GLuint,GLuint);
typedef void (__stdcall *PFNGLDISPATCHCOMPUTEPROC)(GLuint,GLuint,GLuint);
typedef void (__stdcall *PFNGLMEMORYBARRIERPROC)(unsigned int);
typedef void* (__stdcall *PFNGLMAPBUFFERPROC)(unsigned int,unsigned int);
typedef GLboolean (__stdcall *PFNGLUNMAPBUFFERPROC)(unsigned int);
typedef void* GLsync;
typedef GLsync (__stdcall *PFNGLFENCESYNCPROC)(unsigned int,unsigned int);
typedef unsigned int (__stdcall *PFNGLCLIENTWAITSYNCPROC)(GLsync,unsigned int,unsigned long long);
typedef void (__stdcall *PFNGLDELETESYNCPROC)(GLsync);

static PFNGLCREATESHADERPROC pglCreateShader = 0;
static PFNGLSHADERSOURCEPROC pglShaderSource = 0;
static PFNGLCOMPILESHADERPROC pglCompileShader = 0;
static PFNGLGETSHADERIVPROC pglGetShaderiv = 0;
static PFNGLGETSHADERINFOLOGPROC pglGetShaderInfoLog = 0;
static PFNGLCREATEPROGRAMPROC pglCreateProgram = 0;
static PFNGLATTACHSHADERPROC pglAttachShader = 0;
static PFNGLLINKPROGRAMPROC pglLinkProgram = 0;
static PFNGLGETPROGRAMIVPROC pglGetProgramiv = 0;
static PFNGLGETPROGRAMINFOLOGPROC pglGetProgramInfoLog = 0;
static PFNGLUSEPROGRAMPROC pglUseProgram = 0;
static PFNGLGETUNIFORMLOCATIONPROC pglGetUniformLocation = 0;
static PFNGLUNIFORM1IPROC pglUniform1i = 0;
static PFNGLUNIFORM1UIPROC pglUniform1ui = 0;
static PFNGLUNIFORM1FPROC pglUniform1f = 0;
static PFNGLUNIFORM4UIPROC pglUniform4ui = 0;
static PFNGLGENBUFFERSPROC pglGenBuffers = 0;
static PFNGLBINDBUFFERPROC pglBindBuffer = 0;
static PFNGLBUFFERDATAPROC pglBufferData = 0;
static PFNGLBINDBUFFERBASEPROC pglBindBufferBase = 0;
static PFNGLDISPATCHCOMPUTEPROC pglDispatchCompute = 0;
static PFNGLMEMORYBARRIERPROC pglMemoryBarrier = 0;
static PFNGLMAPBUFFERPROC pglMapBuffer = 0;
static PFNGLUNMAPBUFFERPROC pglUnmapBuffer = 0;
static PFNGLFENCESYNCPROC pglFenceSync = 0;
static PFNGLCLIENTWAITSYNCPROC pglClientWaitSync = 0;
static PFNGLDELETESYNCPROC pglDeleteSync = 0;

// Tiny freestanding helpers
extern "C" void* memset(void* dst, int v, unsigned long long n) {
    BYTE* d=(BYTE*)dst; for (unsigned long long i=0;i<n;i++) d[i]=(BYTE)v; return dst;
}
extern "C" void* memcpy(void* dst, const void* src, unsigned long long n) {
    BYTE* d=(BYTE*)dst; const BYTE* s=(const BYTE*)src; for (unsigned long long i=0;i<n;i++) d[i]=s[i]; return dst;
}
extern "C" void* memmove(void* dst, const void* src, unsigned long long n) {
    BYTE* d=(BYTE*)dst; const BYTE* s=(const BYTE*)src;
    if (d<s) for (unsigned long long i=0;i<n;i++) d[i]=s[i];
    else for (unsigned long long i=n;i>0;i--) d[i-1]=s[i-1];
    return dst;
}
static float fclamp(float v,float a,float b){return v<a?a:(v>b?b:v);} 
static int iabs(int v){return v<0?-v:v;}

static wchar_t* wappend(wchar_t* p,const wchar_t* s){while(*s)*p++=*s++;*p=0;return p;}
static wchar_t* wappendInt(wchar_t* p,int v){
    if(v<0){*p++=L'-';v=-v;} wchar_t tmp[16]; int n=0; do{tmp[n++]=(wchar_t)(L'0'+(v%10));v/=10;}while(v);
    while(n>0)*p++=tmp[--n];*p=0;return p;
}
static wchar_t* wappendAscii(wchar_t* p,const unsigned char* s){if(!s)return p; while(*s){unsigned char c=*s++; *p++=(wchar_t)c;}*p=0;return p;}

static double gInvFreq=0.0;
static double nowSeconds(){LARGE_INTEGER q;QueryPerformanceCounter(&q);return (double)q.QuadPart*gInvFreq;}



// ================= TensorCube v32 matrix-free + resource-return execution backend =================
// Every selectable order uses the same implicit topology and stateless component-state provider.
// Ready component ranges are exposed to a persistent CPU executor pool and a GPU compute factorizer
// through one work-conserving ticket space.
// The only global dependency is quotient inference; after it is published, unrelated chunks never wait.

static const int TC_MIN_ORDER=2;
static const int TC_PHYSICAL_RENDER_MAX=10;
static const float TC_CAMERA_BLUR_START_Z=-7.15f;
static const float TC_CAMERA_SAFE_Z=-6.35f;
static const float TC_CAMERA_ZOOM_MAX=-4.20f;
static const int TC_MARKER_STRIDE=144;
static const int TC_MAX_COMPONENT_SIZE=24;
static const int TC_MAX_TEMPLATE_GENS=160;
static const int TC_MACRO_WORD=10;
static const int SG_MAX_DEG=144;
static const int SG_MAX_GENS=320;
static const int SG_MAX_BASE=32;
static const int SG_MAX_WORD=4096;
static const int TC_MAX_CPU_WORKERS=64;
static const int TC_GPU_SLOTS=3;
static const unsigned int TC_CPU_CHUNK_MIN=128u;
static const unsigned int TC_CPU_CHUNK_MAX=8192u;
static const unsigned int TC_GPU_BATCH_MIN=32768u;
static const unsigned int TC_GPU_BATCH_START=131072u;
static const unsigned int TC_GPU_BATCH_MAX=524288u;
static const unsigned int TC_GPU_LOCAL_SIZE=64u;
static const unsigned int TC_GPU_TEMPLATE_TYPES=9u;
static const unsigned int TC_GPU_TEMPLATE_LEVELS=32u;
static const unsigned int TC_GPU_TEMPLATE_DEGREE=144u;
static const unsigned int TC_GPU_TEMPLATE_WORDS=36u;


struct IVec3{int x,y,z;};
struct CubeRotation{IVec3 ex,ey,ez;};
struct Cubie{int id;int x,y,z;int ox,oy,oz;IVec3 bx,by,bz;};
struct BigComponent{unsigned int a,b;BYTE chir,type,size,pad;};
struct CompactFactor{BYTE baseLen;BYTE chosen[SG_MAX_BASE];unsigned int tokenCount;};
struct StreamFactorRecord{unsigned int a,b;BYTE chir,type,size,baseLen;BYTE chosen[SG_MAX_BASE];unsigned int tokenCount;};

struct SPerm{BYTE p[SG_MAX_DEG];};
struct SWord{short len;short tok[SG_MAX_WORD];};
struct BsgsWorkspace{
    int degree,origCount,genCount,baseLen;
    SPerm gen[SG_MAX_GENS];
    SWord word[SG_MAX_GENS];
    int base[SG_MAX_BASE];
    short distCount[SG_MAX_BASE];
    short dist[SG_MAX_BASE][SG_MAX_GENS];
    BYTE transValid[SG_MAX_BASE][SG_MAX_DEG];
    SPerm trans[SG_MAX_BASE][SG_MAX_DEG];
    SWord transWord[SG_MAX_BASE][SG_MAX_DEG];
};
struct MfFactorTemplate{
    int degree,baseLen;
    int base[SG_MAX_BASE];
    BYTE choiceCount[SG_MAX_BASE];
    BYTE choices[SG_MAX_BASE][SG_MAX_DEG];
    BYTE transValid[SG_MAX_BASE][SG_MAX_DEG];
    SPerm trans[SG_MAX_BASE][SG_MAX_DEG];
    SWord transWord[SG_MAX_BASE][SG_MAX_DEG];
};
struct MfDescriptor{BYTE key[32];unsigned int nonce;unsigned int version;BOOL valid;};
struct MfCursor{int m,lo,a,b,ch,cc;unsigned long long ci;BOOL done;};

static HINSTANCE gInst=0;
static HWND gWnd=0,gRenderWnd=0,gOrderCombo=0,gBtnScramble=0,gBtnSolve=0,gBtnPlay=0,gBtnReset=0;
static HWND gPlaybackModeCombo=0,gPlaybackValueEdit=0,gStatus=0,gGpuText=0,gSolverText=0,gHint=0,gPaperText=0;
static HDC gDC=0; static HGLRC gGL=0;
static int gWidth=1320,gHeight=760,gOrder=3;
static float gYaw=-35.0f,gPitch=26.0f,gZoom=-8.4f;
static BOOL gDragging=FALSE;static volatile BOOL gAbortRequested=FALSE;static int gLastX=0,gLastY=0;
static BOOL gMfHasRandom=FALSE,gMfPathReady=FALSE;
static MfDescriptor gMfDesc={{0},0,2,FALSE};
static unsigned long long gMfComponentCount=0,gMfProgress=0,gMfFactorRecordCount=0;
static BYTE* gMfPlanQuotientMask=0;static int gMfQuotientClassCount=0;static int* gMfQuotientMoves=0;static BYTE* gMfQuotientClassBits=0;
static MfFactorTemplate* gMfTemplates[9]={0,0,0,0,0,0,0,0,0};
static StreamFactorRecord gMfPlaybackRecord;static BOOL gMfPlaybackRecordActive=FALSE;
static MfCursor gMfPlaybackCursor;
static HWND gOrderListWnd=0;static WNDPROC gOrderListOldProc=0,gOrderComboOldProc=0;

static CubeRotation gCubeRotations[24];static int gCubeRotationCount=0;
