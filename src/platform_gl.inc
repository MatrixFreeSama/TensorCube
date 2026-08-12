extern "C" __declspec(dllimport) LONG __stdcall BCryptGenRandom(void*,BYTE*,DWORD,DWORD);
// WGL / OpenGL 1.1
extern "C" __declspec(dllimport) HGLRC __stdcall wglCreateContext(HDC); extern "C" __declspec(dllimport) BOOL __stdcall wglMakeCurrent(HDC,HGLRC);
extern "C" __declspec(dllimport) BOOL __stdcall wglDeleteContext(HGLRC); extern "C" __declspec(dllimport) void* __stdcall wglGetProcAddress(LPCSTR);
extern "C" __declspec(dllimport) void __stdcall glClearColor(float,float,float,float); extern "C" __declspec(dllimport) void __stdcall glClearDepth(double);
extern "C" __declspec(dllimport) void __stdcall glClear(unsigned int); extern "C" __declspec(dllimport) void __stdcall glEnable(unsigned int); extern "C" __declspec(dllimport) void __stdcall glDisable(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glDepthFunc(unsigned int); extern "C" __declspec(dllimport) void __stdcall glViewport(int,int,int,int);
extern "C" __declspec(dllimport) void __stdcall glMatrixMode(unsigned int); extern "C" __declspec(dllimport) void __stdcall glLoadIdentity(void);
extern "C" __declspec(dllimport) void __stdcall glFrustum(double,double,double,double,double,double); extern "C" __declspec(dllimport) void __stdcall glTranslatef(float,float,float);
extern "C" __declspec(dllimport) void __stdcall glRotatef(float,float,float,float); extern "C" __declspec(dllimport) void __stdcall glPushMatrix(void); extern "C" __declspec(dllimport) void __stdcall glPopMatrix(void);
extern "C" __declspec(dllimport) void __stdcall glBegin(unsigned int); extern "C" __declspec(dllimport) void __stdcall glEnd(void); extern "C" __declspec(dllimport) void __stdcall glColor3f(float,float,float);
extern "C" __declspec(dllimport) void __stdcall glColor4f(float,float,float,float); extern "C" __declspec(dllimport) void __stdcall glBlendFunc(unsigned int,unsigned int);
extern "C" __declspec(dllimport) void __stdcall glVertex3f(float,float,float); extern "C" __declspec(dllimport) void __stdcall glFlush(void); extern "C" __declspec(dllimport) const unsigned char* __stdcall glGetString(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glGenTextures(int,unsigned int*); extern "C" __declspec(dllimport) void __stdcall glDeleteTextures(int,const unsigned int*); extern "C" __declspec(dllimport) void __stdcall glBindTexture(unsigned int,unsigned int);
extern "C" __declspec(dllimport) void __stdcall glTexParameteri(unsigned int,unsigned int,int); extern "C" __declspec(dllimport) void __stdcall glTexCoord2f(float,float);
extern "C" __declspec(dllimport) void __stdcall glCopyTexImage2D(unsigned int,int,unsigned int,int,int,int,int,int);

extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement=0x00000001UL;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance=1;

extern "C" void* memset(void* d,int v,unsigned long long n){BYTE* p=(BYTE*)d;for(unsigned long long i=0;i<n;i++)p[i]=(BYTE)v;return d;}
extern "C" void* memcpy(void* d,const void* s,unsigned long long n){BYTE* a=(BYTE*)d;const BYTE*b=(const BYTE*)s;for(unsigned long long i=0;i<n;i++)a[i]=b[i];return d;}

// Win32 constants copied from v32 shell
static const UINT CS_OWNDC=0x0020; static const DWORD WS_OVERLAPPEDWINDOW=0x00CF0000UL,WS_VISIBLE=0x10000000UL,WS_CHILD=0x40000000UL,WS_TABSTOP=0x00010000UL,WS_VSCROLL=0x00200000UL;
static const DWORD BS_PUSHBUTTON=0,SS_LEFT=0,CBS_DROPDOWNLIST=0x0003UL,ES_AUTOHSCROLL=0x0080UL;
static const int CW_USEDEFAULT=(int)0x80000000u,SW_SHOW=5; static const UINT WM_DESTROY=0x0002,WM_SIZE=0x0005,WM_COMMAND=0x0111,WM_KEYDOWN=0x0100,WM_LBUTTONDOWN=0x0201,WM_LBUTTONUP=0x0202,WM_MOUSEMOVE=0x0200,WM_MOUSEWHEEL=0x020A;
static const UINT PM_REMOVE=1,VK_ESCAPE=0x1B,VK_PRIOR=0x21,VK_NEXT=0x22,CBN_SELCHANGE=1,CB_ADDSTRING=0x0143,CB_GETCURSEL=0x0147,CB_SETCURSEL=0x014E;
static const UINT MB_OK=0,MB_ICONERROR=0x10; static const DWORD PFD_DOUBLEBUFFER=1,PFD_DRAW_TO_WINDOW=4,PFD_SUPPORT_OPENGL=0x20,BCRYPT_USE_SYSTEM_PREFERRED_RNG=2;
static const BYTE PFD_TYPE_RGBA=0,PFD_MAIN_PLANE=0; static const DWORD INFINITE=0xffffffffUL; static const WORD ALL_PROCESSOR_GROUPS=0xffffu;
// GL constants copied from v32 shell
static const unsigned int GL_COLOR_BUFFER_BIT=0x4000,GL_DEPTH_BUFFER_BIT=0x0100,GL_DEPTH_TEST=0x0B71,GL_LEQUAL=0x0203,GL_PROJECTION=0x1701,GL_MODELVIEW=0x1700,GL_QUADS=0x0007,GL_TRIANGLE_FAN=0x0006;
static const unsigned int GL_RENDERER=0x1F01,GL_VERSION=0x1F02,GL_BLEND=0x0BE2,GL_SRC_ALPHA=0x0302,GL_ONE_MINUS_SRC_ALPHA=0x0303,GL_TEXTURE_2D=0x0DE1,GL_TEXTURE_MIN_FILTER=0x2801,GL_TEXTURE_MAG_FILTER=0x2800,GL_LINEAR=0x2601,GL_TEXTURE_WRAP_S=0x2802,GL_TEXTURE_WRAP_T=0x2803,GL_CLAMP=0x2900,GL_RGBA=0x1908;
static const int WGL_CONTEXT_MAJOR_VERSION_ARB=0x2091,WGL_CONTEXT_MINOR_VERSION_ARB=0x2092,WGL_CONTEXT_PROFILE_MASK_ARB=0x9126,WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB=2;
typedef HGLRC (__stdcall *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);

static HINSTANCE gInst=0; static HWND gWnd=0,gRenderWnd=0; static HDC gDC=0; static HGLRC gGL=0; static int gWidth=1200,gHeight=700; static BOOL gAbortRequested=FALSE;
static HWND gOrderCombo=0,gBtnScramble=0,gBtnSolve=0,gBtnPlay=0,gBtnReset=0,gPlaybackModeCombo=0,gPlaybackValueEdit=0,gBtnPrevPage=0,gBtnNextPage=0,gViewPageText=0,gStatus=0,gGpuText=0,gSolverText=0,gHint=0,gPaperText=0;
static wchar_t gStatusCache[1800]=L"",gGpuCache[1900]=L"",gSolverCache[2300]=L"",gPaperCache[2400]=L"",gPageCache[256]=L"";

static float fclamp(float v,float a,float b){return v<a?a:(v>b?b:v);} 
static wchar_t* wappend(wchar_t* p,const wchar_t* s){while(s&&*s)*p++=*s++;*p=0;return p;}
static wchar_t* wappendInt(wchar_t* p,int v){if(v<0){*p++=L'-';v=-v;}wchar_t t[32];int n=0;do{t[n++]=(wchar_t)(L'0'+v%10);v/=10;}while(v);while(n)*p++=t[--n];*p=0;return p;}
static wchar_t* wappendULL(wchar_t* p,unsigned long long v){wchar_t t[32];int n=0;do{t[n++]=(wchar_t)(L'0'+v%10ull);v/=10ull;}while(v);while(n)*p++=t[--n];*p=0;return p;}
static wchar_t* wappendFixed2(wchar_t* p,double v){if(v<0){*p++=L'-';v=-v;}unsigned long long whole=(unsigned long long)v;p=wappendULL(p,whole);*p++=L'.';int h=(int)((v-(double)whole)*100.0+0.5);if(h>=100)h=99;*p++=(wchar_t)(L'0'+h/10);*p++=(wchar_t)(L'0'+h%10);*p=0;return p;}
static wchar_t* wappendAscii(wchar_t* p,const unsigned char* s){if(!s)return wappend(p,L"unknown");while(*s)*p++=(wchar_t)*s++;*p=0;return p;}
static BOOL tcWstrEq(const wchar_t* a,const wchar_t* b){while(*a&&*b){if(*a++!=*b++)return FALSE;}return *a==*b;}
static void tcCopyW(wchar_t* d,int cap,const wchar_t* s){int i=0;if(cap<=0)return;while(s&&*s&&i<cap-1)d[i++]=*s++;d[i]=0;}
static void stableSetText(HWND h,wchar_t* cache,int cap,const wchar_t* text){if(!h||tcWstrEq(cache,text))return;tcCopyW(cache,cap,text);SetWindowTextW(h,text);}
static double gInvFreq=0.0; static double nowSeconds(){LARGE_INTEGER q;QueryPerformanceCounter(&q);return (double)q.QuadPart*gInvFreq;}


// -----------------------------------------------------------------------------
