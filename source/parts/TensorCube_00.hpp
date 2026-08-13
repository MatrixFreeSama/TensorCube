// TensorCube v32, portable Windows build for NxNxN cubes.
// Selectable orders: 2..49, plus stress extensions 100, 1000, 10000 and 100000.
// Freestanding Win32 + OpenGL, no external runtime dependencies.
// One matrix-free orbit-tensor provider and streaming factorization backend is shared by every selectable order.
// OpenGL 4.3 Compute performs matrix-free component factorization; a persistent CPU pool shares ready work.
// Random state is an implicit deterministic tensor descriptor; no generation history or task files are retained.
// Execution follows a work-conserving resource-return model: logical work is detached from executor identity,
// only the quotient reduction is a global dependency, and unrelated component chunks never wait for one another.

#define WIN32_LEAN_AND_MEAN

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef ULONGLONG UINT_PTR;
typedef LONGLONG LONG_PTR;
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef void* HANDLE;
typedef HANDLE HWND;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HBRUSH;
typedef HANDLE HMENU;
typedef HANDLE HDC;
typedef HANDLE HGLRC;
typedef const wchar_t* LPCWSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef void* LPVOID;
typedef unsigned short ATOM;
typedef int BOOL;

#ifndef NULL
#define NULL 0
#endif
#define TRUE 1
#define FALSE 0

struct POINT { LONG x; LONG y; };
struct RECT { LONG left, top, right, bottom; };
struct COMBOBOXINFO { DWORD cbSize; RECT rcItem; RECT rcButton; DWORD stateButton; HWND hwndCombo; HWND hwndItem; HWND hwndList; };
struct MSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; DWORD lPrivate; };
struct LARGE_INTEGER { LONGLONG QuadPart; };

typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);
struct WNDCLASSW {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
};

struct PIXELFORMATDESCRIPTOR {
    WORD nSize;
    WORD nVersion;
    DWORD dwFlags;
    BYTE iPixelType;
    BYTE cColorBits;
    BYTE cRedBits;
    BYTE cRedShift;
    BYTE cGreenBits;
    BYTE cGreenShift;
    BYTE cBlueBits;
    BYTE cBlueShift;
    BYTE cAlphaBits;
    BYTE cAlphaShift;
    BYTE cAccumBits;
    BYTE cAccumRedBits;
    BYTE cAccumGreenBits;
    BYTE cAccumBlueBits;
    BYTE cAccumAlphaBits;
    BYTE cDepthBits;
    BYTE cStencilBits;
    BYTE cAuxBuffers;
    BYTE iLayerType;
    BYTE bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
};

extern "C" int _fltused = 0;
extern "C" void __chkstk(void) {}

// user32
extern "C" __declspec(dllimport) ATOM __stdcall RegisterClassW(const WNDCLASSW*);
extern "C" __declspec(dllimport) HWND __stdcall CreateWindowExW(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
extern "C" __declspec(dllimport) LRESULT __stdcall DefWindowProcW(HWND,UINT,WPARAM,LPARAM);
extern "C" __declspec(dllimport) BOOL __stdcall ShowWindow(HWND,int);
extern "C" __declspec(dllimport) BOOL __stdcall UpdateWindow(HWND);
extern "C" __declspec(dllimport) BOOL __stdcall PeekMessageW(MSG*,HWND,UINT,UINT,UINT);
extern "C" __declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG*);
extern "C" __declspec(dllimport) LRESULT __stdcall DispatchMessageW(const MSG*);
extern "C" __declspec(dllimport) void __stdcall PostQuitMessage(int);
extern "C" __declspec(dllimport) BOOL __stdcall GetClientRect(HWND,RECT*);
extern "C" __declspec(dllimport) BOOL __stdcall SetWindowTextW(HWND,LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall EnableWindow(HWND,BOOL);
extern "C" __declspec(dllimport) HWND __stdcall SetCapture(HWND);
extern "C" __declspec(dllimport) BOOL __stdcall ReleaseCapture(void);
extern "C" __declspec(dllimport) BOOL __stdcall DestroyWindow(HWND);
extern "C" __declspec(dllimport) HCURSOR __stdcall LoadCursorW(HINSTANCE,LPCWSTR);
extern "C" __declspec(dllimport) HICON __stdcall LoadIconW(HINSTANCE,LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall SetProcessDPIAware(void);
extern "C" __declspec(dllimport) BOOL __stdcall MoveWindow(HWND,int,int,int,int,BOOL);
extern "C" __declspec(dllimport) int __stdcall MessageBoxW(HWND,LPCWSTR,LPCWSTR,UINT);
extern "C" __declspec(dllimport) int __stdcall GetWindowTextW(HWND,LPWSTR,int);
extern "C" __declspec(dllimport) LRESULT __stdcall SendMessageW(HWND,UINT,WPARAM,LPARAM);
extern "C" __declspec(dllimport) BOOL __stdcall GetComboBoxInfo(HWND,COMBOBOXINFO*);
extern "C" __declspec(dllimport) LONG_PTR __stdcall SetWindowLongPtrW(HWND,int,LONG_PTR);
extern "C" __declspec(dllimport) LRESULT __stdcall CallWindowProcW(WNDPROC,HWND,UINT,WPARAM,LPARAM);

// user32 DC access
extern "C" __declspec(dllimport) HDC __stdcall GetDC(HWND);
extern "C" __declspec(dllimport) int __stdcall ReleaseDC(HWND,HDC);

// gdi32
extern "C" __declspec(dllimport) int __stdcall ChoosePixelFormat(HDC,const PIXELFORMATDESCRIPTOR*);
extern "C" __declspec(dllimport) BOOL __stdcall SetPixelFormat(HDC,int,const PIXELFORMATDESCRIPTOR*);
extern "C" __declspec(dllimport) BOOL __stdcall SwapBuffers(HDC);

// kernel32
extern "C" __declspec(dllimport) void __stdcall ExitProcess(UINT);
extern "C" __declspec(dllimport) HINSTANCE __stdcall GetModuleHandleW(LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter(LARGE_INTEGER*);
extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(LARGE_INTEGER*);
extern "C" __declspec(dllimport) void __stdcall Sleep(DWORD);
extern "C" __declspec(dllimport) LPVOID __stdcall VirtualAlloc(LPVOID,unsigned long long,DWORD,DWORD);
extern "C" __declspec(dllimport) BOOL __stdcall VirtualFree(LPVOID,unsigned long long,DWORD);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID,unsigned long long,DWORD (__stdcall *)(LPVOID),LPVOID,DWORD,DWORD*);
extern "C" __declspec(dllimport) HANDLE __stdcall CreateSemaphoreW(LPVOID,LONG,LONG,LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE,LONG,LONG*);
extern "C" __declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE,DWORD);
extern "C" __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
extern "C" __declspec(dllimport) DWORD __stdcall GetActiveProcessorCount(WORD);
extern "C" __declspec(dllimport) HMODULE __stdcall LoadLibraryW(LPCWSTR);
extern "C" __declspec(dllimport) void* __stdcall GetProcAddress(HMODULE,LPCSTR);
extern "C" __declspec(dllimport) BOOL __stdcall FreeLibrary(HMODULE);

// Compiler-resident atomics. These deliberately do not import Interlocked* symbols from KERNEL32.
// On x86-64 Clang lowers these operations directly to LOCK/XCHG instructions.
static __forceinline LONGLONG tcAtomicExchangeAdd64(volatile LONGLONG* p,LONGLONG v){
    return __atomic_fetch_add((LONGLONG*)p,v,__ATOMIC_SEQ_CST);
}
static __forceinline LONGLONG tcAtomicExchange64(volatile LONGLONG* p,LONGLONG v){
    return __atomic_exchange_n((LONGLONG*)p,v,__ATOMIC_SEQ_CST);
}
static __forceinline LONGLONG tcAtomicCompareExchange64(volatile LONGLONG* p,LONGLONG exchange,LONGLONG comparand){
    LONGLONG expected=comparand;
    __atomic_compare_exchange_n((LONGLONG*)p,&expected,exchange,FALSE,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
    return expected;
}
static __forceinline LONG tcAtomicIncrement(volatile LONG* p){
    return __atomic_add_fetch((LONG*)p,1,__ATOMIC_SEQ_CST);
}
static __forceinline LONG tcAtomicExchange(volatile LONG* p,LONG v){
    return __atomic_exchange_n((LONG*)p,v,__ATOMIC_SEQ_CST);
}


extern "C" void* memset(void*,int,unsigned long long);
static void* valloc(unsigned long long);
static void vfree(void*);

// NVIDIA Tensor Core assist lane.
// It uses only the NVIDIA display driver's CUDA Driver API (nvcuda.dll), loaded dynamically.
// No CUDA Toolkit runtime DLL is required.  The embedded PTX contains a real integer WMMA instruction.
// The production use is exact reduction of GPU factor-batch token/record/error counters.
// Core permutation/BSGS factorization remains on CPU workers and OpenGL compute because those operations are gathers/branches,
// not dense matrix multiplies; they are deliberately not inflated into fake GEMMs.
typedef int CUresult;
typedef int CUdevice;
typedef unsigned long long CUdeviceptr;
typedef void* CUcontext;
typedef void* CUmodule;
typedef void* CUfunction;
typedef void* CUstream;
static const CUresult CUDA_SUCCESS=0;
typedef CUresult (__stdcall *PFNCUINIT)(unsigned int);
typedef CUresult (__stdcall *PFNCUDEVICEGET)(CUdevice*,int);
typedef CUresult (__stdcall *PFNCUDEVICEPRIMARYCTXRETAIN)(CUcontext*,CUdevice);
typedef CUresult (__stdcall *PFNCUDEVICEPRIMARYCTXRELEASE)(CUdevice);
