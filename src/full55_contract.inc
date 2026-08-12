// 11D ORDER-4 FULL55 SOURCE-MACRO TENSOR LIFT
//
// This backend removes the old disjoint 3D-fiber benchmark substitution.
// All C(11,2)=55 coordinate rotation planes participate in one authoritative
// full-surface state family.  Legal generators are dimension-lifted directly
// from the archived source TensorCube order-4 macro family (types 1/4/5).
//
// Randomization contract:
//   Windows CSPRNG -> source-style ChaCha20 (256-bit key + 32-bit nonce)
//   -> independent uniform even piece permutation on every full 11D surface
//      orbit -> canonical legal source-macro signed-orientation lift.
// The key, nonce and RNG buffer are securely destroyed after publication.
// No scramble word/path is retained.
//
// Solve contract:
//   authoritative current state only -> recover current orbit permutation
//   -> sparse star factorization over the public source-macro basis
//   -> inverse legal factor word -> independent full position + signed-11D
//      orientation verification.  The factorizer has no RNG/descriptor access.
//
// Academic scope: this is the canonical source-macro signed-orientation lift.
// Arbitrary pure orientation-kernel states are explicitly NOT claimed.
// -----------------------------------------------------------------------------
typedef short i16;
struct Config{int d,n;};
struct Normal{int axis;i8 sign;};
struct SolveStats{double seconds;u64 factors,primitive;BOOL exact;int workers;BOOL gpu;};

static Config gCfg={11,4};
static volatile BOOL gScrambled=0,gSolving=0,gPlanReady=0,gPlaying=0;
static SolveStats gStats={};
static int gCPUWorkers=1;
static double gPlayDuration=8.0,gPlayProgress=0.0,gPlayStart=0.0;
static u64 gHiddenSlice=0;

static u64 ipow(int b,int e){u64 r=1,x=(u64)b;while(e){if(e&1)r*=x;e>>=1;if(e)x*=x;}return r;}
static u64 surfaceSites(const Config& c){return ipow(c.n,c.d)-ipow(c.n-2,c.d);}
static int planeCount(const Config& c){return c.d*(c.d-1)/2;}
static u64 layersPerPlane(const Config& c){return ipow(c.n,c.d-2);}
static u64 legalLayerDescriptors(const Config& c){return (u64)planeCount(c)*layersPerPlane(c);}
static void decodeIndex4(u32 idx,u8*c){for(int a=0;a<11;a++){c[a]=(u8)(idx&3u);idx>>=2;}}
static u32 encodeIndex4(const u8*c){u32 x=0;for(int a=10;a>=0;a--)x=(x<<2)|(u32)(c[a]&3u);return x;}
static int solvedFace(const Config&cfg,const u8*c,Normal n){if(n.axis<0||n.axis>=cfg.d)return -1;if(n.sign>0&&c[n.axis]==(u8)(cfg.n-1))return 2*n.axis;if(n.sign<0&&c[n.axis]==0)return 2*n.axis+1;return -1;}
static void secureZero(void*ptr,unsigned long long n){volatile BYTE*p=(volatile BYTE*)ptr;while(n--)*p++=0;}

#include "full55_core_freestanding.hpp"
