// TensorCube 11D Order-4 source-macro tensor lift.
// All 55 coordinate planes participate in the authoritative full-surface state.
// The solver reads state only and cannot access randomization secrets or history.
// Win32 UI, render child, camera, studio presentation, cubie/sticker geometry,
// playback controls and interaction shell follow TensorCube v32.

#define WIN32_LEAN_AND_MEAN

typedef unsigned char BYTE; typedef unsigned short WORD; typedef unsigned int UINT; typedef unsigned long DWORD;
typedef long LONG; typedef long long LONGLONG; typedef unsigned long long ULONGLONG; typedef ULONGLONG UINT_PTR;
typedef LONGLONG LONG_PTR; typedef UINT_PTR WPARAM; typedef LONG_PTR LPARAM; typedef LONG_PTR LRESULT;
typedef void* HANDLE; typedef HANDLE HWND; typedef HANDLE HINSTANCE; typedef HANDLE HMODULE; typedef HANDLE HICON;
typedef HANDLE HCURSOR; typedef HANDLE HBRUSH; typedef HANDLE HMENU; typedef HANDLE HDC; typedef HANDLE HGLRC;
typedef const wchar_t* LPCWSTR; typedef const char* LPCSTR; typedef wchar_t* LPWSTR; typedef void* LPVOID;
typedef unsigned short ATOM; typedef int BOOL;
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32; typedef unsigned long long u64;
typedef signed char i8; typedef int i32; typedef long long i64; typedef unsigned long ULONG; typedef unsigned long long SIZE_T;

#ifndef NULL
#define NULL 0
#endif
#define TRUE 1
#define FALSE 0

struct POINT{LONG x,y;}; struct RECT{LONG left,top,right,bottom;};
struct COMBOBOXINFO{DWORD cbSize;RECT rcItem;RECT rcButton;DWORD stateButton;HWND hwndCombo,hwndItem,hwndList;};
struct MSG{HWND hwnd;UINT message;WPARAM wParam;LPARAM lParam;DWORD time;POINT pt;DWORD lPrivate;};
struct LARGE_INTEGER{LONGLONG QuadPart;};
typedef LRESULT (__stdcall *WNDPROC)(HWND,UINT,WPARAM,LPARAM);
struct WNDCLASSW{UINT style;WNDPROC lpfnWndProc;int cbClsExtra,cbWndExtra;HINSTANCE hInstance;HICON hIcon;HCURSOR hCursor;HBRUSH hbrBackground;LPCWSTR lpszMenuName,lpszClassName;};
struct PIXELFORMATDESCRIPTOR{WORD nSize,nVersion;DWORD dwFlags;BYTE iPixelType,cColorBits,cRedBits,cRedShift,cGreenBits,cGreenShift,cBlueBits,cBlueShift,cAlphaBits,cAlphaShift,cAccumBits,cAccumRedBits,cAccumGreenBits,cAccumBlueBits,cAccumAlphaBits,cDepthBits,cStencilBits,cAuxBuffers,iLayerType,bReserved;DWORD dwLayerMask,dwVisibleMask,dwDamageMask;};

extern "C" int _fltused=0; extern "C" void __chkstk(void){}

// user32
extern "C" __declspec(dllimport) ATOM __stdcall RegisterClassW(const WNDCLASSW*);
extern "C" __declspec(dllimport) HWND __stdcall CreateWindowExW(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
extern "C" __declspec(dllimport) LRESULT __stdcall DefWindowProcW(HWND,UINT,WPARAM,LPARAM);
extern "C" __declspec(dllimport) BOOL __stdcall ShowWindow(HWND,int); extern "C" __declspec(dllimport) BOOL __stdcall UpdateWindow(HWND);
extern "C" __declspec(dllimport) BOOL __stdcall PeekMessageW(MSG*,HWND,UINT,UINT,UINT); extern "C" __declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG*);
extern "C" __declspec(dllimport) LRESULT __stdcall DispatchMessageW(const MSG*); extern "C" __declspec(dllimport) void __stdcall PostQuitMessage(int);
extern "C" __declspec(dllimport) BOOL __stdcall GetClientRect(HWND,RECT*); extern "C" __declspec(dllimport) BOOL __stdcall SetWindowTextW(HWND,LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall EnableWindow(HWND,BOOL); extern "C" __declspec(dllimport) HWND __stdcall SetCapture(HWND);
extern "C" __declspec(dllimport) BOOL __stdcall ReleaseCapture(void); extern "C" __declspec(dllimport) BOOL __stdcall DestroyWindow(HWND);
extern "C" __declspec(dllimport) HCURSOR __stdcall LoadCursorW(HINSTANCE,LPCWSTR); extern "C" __declspec(dllimport) HICON __stdcall LoadIconW(HINSTANCE,LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall SetProcessDPIAware(void); extern "C" __declspec(dllimport) BOOL __stdcall MoveWindow(HWND,int,int,int,int,BOOL);
extern "C" __declspec(dllimport) int __stdcall MessageBoxW(HWND,LPCWSTR,LPCWSTR,UINT); extern "C" __declspec(dllimport) int __stdcall GetWindowTextW(HWND,LPWSTR,int);
extern "C" __declspec(dllimport) LRESULT __stdcall SendMessageW(HWND,UINT,WPARAM,LPARAM); extern "C" __declspec(dllimport) BOOL __stdcall GetComboBoxInfo(HWND,COMBOBOXINFO*);
extern "C" __declspec(dllimport) LONG_PTR __stdcall SetWindowLongPtrW(HWND,int,LONG_PTR); extern "C" __declspec(dllimport) LRESULT __stdcall CallWindowProcW(WNDPROC,HWND,UINT,WPARAM,LPARAM);
extern "C" __declspec(dllimport) HDC __stdcall GetDC(HWND); extern "C" __declspec(dllimport) int __stdcall ReleaseDC(HWND,HDC);
// gdi32
extern "C" __declspec(dllimport) int __stdcall ChoosePixelFormat(HDC,const PIXELFORMATDESCRIPTOR*); extern "C" __declspec(dllimport) BOOL __stdcall SetPixelFormat(HDC,int,const PIXELFORMATDESCRIPTOR*); extern "C" __declspec(dllimport) BOOL __stdcall SwapBuffers(HDC);
// kernel32
extern "C" __declspec(dllimport) void __stdcall ExitProcess(UINT); extern "C" __declspec(dllimport) HINSTANCE __stdcall GetModuleHandleW(LPCWSTR);
extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter(LARGE_INTEGER*); extern "C" __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(LARGE_INTEGER*);
extern "C" __declspec(dllimport) void __stdcall Sleep(DWORD); extern "C" __declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID,unsigned long long,DWORD (__stdcall *)(LPVOID),LPVOID,DWORD,DWORD*);
extern "C" __declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE,DWORD); extern "C" __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
extern "C" __declspec(dllimport) DWORD __stdcall GetActiveProcessorCount(WORD); extern "C" __declspec(dllimport) HMODULE __stdcall LoadLibraryW(LPCWSTR);
extern "C" __declspec(dllimport) void* __stdcall GetProcAddress(HMODULE,LPCSTR); extern "C" __declspec(dllimport) BOOL __stdcall FreeLibrary(HMODULE);
// bcrypt
