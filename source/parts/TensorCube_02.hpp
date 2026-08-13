        gTcHostA[(tile+tiles*5u)*256u+pos]=(BYTE)(bv&255u);
    }
    if(!tcCudaOk(pcuCtxSetCurrent(gTcCtx))||!tcCudaOk(pcuMemcpyHtoD(gTcA,gTcHostA,(unsigned long long)mats*256ull))||
       !tensorCoreLaunchMatrices(mats)||!tcCudaOk(pcuMemcpyDtoH(gTcHostD,gTcD,(unsigned long long)mats*1024ull))){
        gTensorCoreReductionActive=FALSE;setTensorCoreDiag(L"WMMA runtime reduction disabled after CUDA failure");return FALSE;
    }
    unsigned long long plane[6]={0,0,0,0,0,0};
    for(unsigned int m=0;m<mats;m++){
        unsigned long long sum=0;int* d=gTcHostD+(unsigned long long)m*256ull;
        for(int row=0;row<16;row++)sum+=(unsigned long long)(unsigned int)d[row*16];
        plane[m/tiles]+=sum;
    }
    *tokens=plane[0]+(plane[1]<<8)+(plane[2]<<16)+(plane[3]<<24);
    *records=plane[4];*bad=plane[5];return TRUE;
}

// bcrypt: system RNG for the RAM-only matrix-free random tensor descriptor. No registry APIs are used.
extern "C" __declspec(dllimport) LONG __stdcall BCryptGenRandom(void*,BYTE*,DWORD,DWORD);

// OpenGL 1.1 / WGL
extern "C" __declspec(dllimport) HGLRC __stdcall wglCreateContext(HDC);
extern "C" __declspec(dllimport) BOOL __stdcall wglMakeCurrent(HDC,HGLRC);
extern "C" __declspec(dllimport) BOOL __stdcall wglDeleteContext(HGLRC);
extern "C" __declspec(dllimport) void* __stdcall wglGetProcAddress(LPCSTR);

extern "C" __declspec(dllimport) void __stdcall glClearColor(float,float,float,float);
extern "C" __declspec(dllimport) void __stdcall glClearDepth(double);
extern "C" __declspec(dllimport) void __stdcall glClear(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glEnable(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glDisable(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glDepthFunc(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glViewport(int,int,int,int);
extern "C" __declspec(dllimport) void __stdcall glMatrixMode(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glLoadIdentity(void);
extern "C" __declspec(dllimport) void __stdcall glFrustum(double,double,double,double,double,double);
extern "C" __declspec(dllimport) void __stdcall glTranslatef(float,float,float);
extern "C" __declspec(dllimport) void __stdcall glRotatef(float,float,float,float);
extern "C" __declspec(dllimport) void __stdcall glMultMatrixf(const float*);
extern "C" __declspec(dllimport) void __stdcall glPushMatrix(void);
extern "C" __declspec(dllimport) void __stdcall glPopMatrix(void);
extern "C" __declspec(dllimport) void __stdcall glBegin(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glEnd(void);
extern "C" __declspec(dllimport) void __stdcall glColor3f(float,float,float);
extern "C" __declspec(dllimport) void __stdcall glColor4f(float,float,float,float);
extern "C" __declspec(dllimport) void __stdcall glBlendFunc(unsigned int,unsigned int);
extern "C" __declspec(dllimport) void __stdcall glVertex3f(float,float,float);
extern "C" __declspec(dllimport) void __stdcall glFlush(void);
extern "C" __declspec(dllimport) const unsigned char* __stdcall glGetString(unsigned int);
extern "C" __declspec(dllimport) void __stdcall glGenTextures(int,unsigned int*);
extern "C" __declspec(dllimport) void __stdcall glDeleteTextures(int,const unsigned int*);
extern "C" __declspec(dllimport) void __stdcall glBindTexture(unsigned int,unsigned int);
extern "C" __declspec(dllimport) void __stdcall glTexParameteri(unsigned int,unsigned int,int);
extern "C" __declspec(dllimport) void __stdcall glTexImage2D(unsigned int,int,int,int,int,int,unsigned int,unsigned int,const void*);
extern "C" __declspec(dllimport) void __stdcall glTexSubImage2D(unsigned int,int,int,int,int,int,unsigned int,unsigned int,const void*);
extern "C" __declspec(dllimport) void __stdcall glTexCoord2f(float,float);
extern "C" __declspec(dllimport) void __stdcall glPixelStorei(unsigned int,int);
extern "C" __declspec(dllimport) void __stdcall glCopyTexImage2D(unsigned int,int,unsigned int,int,int,int,int,int);

// Request the high-performance discrete GPU on hybrid Windows laptops.
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001UL;
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

// Win32 constants
static const UINT CS_OWNDC = 0x0020;
static const DWORD WS_OVERLAPPEDWINDOW = 0x00CF0000UL;
static const DWORD WS_VISIBLE = 0x10000000UL;
static const DWORD WS_CHILD = 0x40000000UL;
static const DWORD WS_TABSTOP = 0x00010000UL;
static const DWORD WS_DISABLED = 0x08000000UL;
static const DWORD WS_VSCROLL = 0x00200000UL;
static const DWORD BS_PUSHBUTTON = 0x00000000UL;
static const DWORD SS_LEFT = 0x00000000UL;
static const int CW_USEDEFAULT = (int)0x80000000u;
static const int SW_SHOW = 5;
static const UINT WM_DESTROY = 0x0002;
static const UINT WM_SIZE = 0x0005;
static const UINT WM_COMMAND = 0x0111;
static const UINT WM_KEYDOWN = 0x0100;
static const UINT WM_LBUTTONDOWN = 0x0201;
static const UINT WM_LBUTTONUP = 0x0202;
static const UINT WM_MOUSEMOVE = 0x0200;
static const UINT WM_MOUSEWHEEL = 0x020A;
static const UINT PM_REMOVE = 0x0001;
static const UINT VK_ESCAPE = 0x1B;
static const UINT MB_OK = 0x00000000u;
static const UINT MB_ICONERROR = 0x00000010u;
static const DWORD PFD_DOUBLEBUFFER = 0x00000001UL;
static const DWORD PFD_DRAW_TO_WINDOW = 0x00000004UL;
static const DWORD BCRYPT_USE_SYSTEM_PREFERRED_RNG = 0x00000002UL;
static const DWORD PFD_SUPPORT_OPENGL = 0x00000020UL;
static const BYTE PFD_TYPE_RGBA = 0;
static const BYTE PFD_MAIN_PLANE = 0;

static const DWORD CBS_DROPDOWNLIST = 0x0003UL;
static const DWORD ES_AUTOHSCROLL = 0x0080UL;
static const UINT CBN_SELCHANGE = 1;
static const UINT CB_ADDSTRING = 0x0143;
static const UINT CB_GETCURSEL = 0x0147;
static const UINT CB_SETCURSEL = 0x014E;
static const UINT CB_GETDROPPEDSTATE = 0x0157;
static const UINT LB_GETCOUNT = 0x018B;
static const UINT LB_GETTOPINDEX = 0x018E;
static const UINT LB_SETTOPINDEX = 0x0197;
static const int GWL_WNDPROC = -4;
static const DWORD MEM_COMMIT = 0x1000UL;
static const DWORD MEM_RESERVE = 0x2000UL;
static const DWORD MEM_RELEASE = 0x8000UL;
static const DWORD PAGE_READWRITE = 0x04UL;
static const DWORD INFINITE = 0xffffffffUL;
static const DWORD WAIT_OBJECT_0 = 0x00000000UL;
static const WORD ALL_PROCESSOR_GROUPS = 0xffffu;

// GL constants
static const unsigned int GL_COLOR_BUFFER_BIT = 0x00004000;
static const unsigned int GL_DEPTH_BUFFER_BIT = 0x00000100;
static const unsigned int GL_DEPTH_TEST = 0x0B71;
static const unsigned int GL_LEQUAL = 0x0203;
static const unsigned int GL_PROJECTION = 0x1701;
static const unsigned int GL_MODELVIEW = 0x1700;
static const unsigned int GL_QUADS = 0x0007;
static const unsigned int GL_TRIANGLE_FAN = 0x0006;
static const unsigned int GL_RENDERER = 0x1F01;
static const unsigned int GL_VERSION = 0x1F02;
static const unsigned int GL_COMPUTE_SHADER = 0x91B9;
static const unsigned int GL_VERTEX_SHADER = 0x8B31;
static const unsigned int GL_FRAGMENT_SHADER = 0x8B30;
static const unsigned int GL_COMPILE_STATUS = 0x8B81;
static const unsigned int GL_LINK_STATUS = 0x8B82;
static const unsigned int GL_SHADER_STORAGE_BUFFER = 0x90D2;
static const unsigned int GL_DYNAMIC_DRAW = 0x88E8;
static const unsigned int GL_READ_ONLY = 0x88B8;
static const unsigned int GL_SHADER_STORAGE_BARRIER_BIT = 0x00002000;
static const unsigned int GL_BUFFER_UPDATE_BARRIER_BIT = 0x00000200;
static const unsigned int GL_STATIC_DRAW = 0x88E4;
static const unsigned int GL_SYNC_GPU_COMMANDS_COMPLETE = 0x9117;
static const unsigned int GL_ALREADY_SIGNALED = 0x911A;
static const unsigned int GL_TIMEOUT_EXPIRED = 0x911B;
static const unsigned int GL_CONDITION_SATISFIED = 0x911C;
static const unsigned int GL_WAIT_FAILED = 0x911D;
static const unsigned int GL_BLEND = 0x0BE2;
static const unsigned int GL_SRC_ALPHA = 0x0302;
static const unsigned int GL_ONE_MINUS_SRC_ALPHA = 0x0303;
static const unsigned int GL_TEXTURE_2D = 0x0DE1;
static const unsigned int GL_TEXTURE_MIN_FILTER = 0x2801;
static const unsigned int GL_TEXTURE_MAG_FILTER = 0x2800;
static const unsigned int GL_NEAREST = 0x2600;
static const unsigned int GL_LINEAR = 0x2601;
static const unsigned int GL_TEXTURE_WRAP_S = 0x2802;
static const unsigned int GL_TEXTURE_WRAP_T = 0x2803;
static const unsigned int GL_CLAMP = 0x2900;
static const unsigned int GL_REPEAT = 0x2901;
static const unsigned int GL_RGBA = 0x1908;
static const unsigned int GL_UNSIGNED_BYTE = 0x1401;
static const unsigned int GL_UNPACK_ALIGNMENT = 0x0CF5;

// WGL_ARB_create_context constants.  A temporary legacy context is used only
// to load wglCreateContextAttribsARB, then the renderer explicitly requests a 4.3+
// compatibility profile so the existing immediate-mode renderer and compute
// shader can coexist in the same context.
static const int WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
static const int WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;
static const int WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;
