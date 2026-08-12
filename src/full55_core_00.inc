
#ifndef TC_TYPES_DEFINED
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32; typedef unsigned long long u64; typedef signed char i8;
#endif

static const int F55_D=11;
static const u32 F55_TOTAL=(1u<<(2*F55_D));
static const u32 F55_INTERIOR=(1u<<F55_D);
static const u32 F55_SURFACE=F55_TOTAL-F55_INTERIOR;
static const u32 F55_BAD=0xffffffffu;
static const u32 F55_MAX_ORBIT=946176u;
static const u32 F55_MAX_QUEUE=2u*F55_MAX_ORBIT+512u;
static const u32 F55_MAX_PLAN=4u*F55_SURFACE+128u;
static const u32 F55_ORBIT_OFF[12]={0,0,22528,135168,473088,1148928,2095104,3041280,3717120,4055040,4167680,4190208};
static const u32 F55_ORBIT_N[12]={0,22528,112640,337920,675840,946176,946176,675840,337920,112640,22528,2048};
static const int F55_AXPERM[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};

#include "full55_local_cycles.inc"

struct F55_Axes3{u8 a[3];};
struct F55_Opt{u8 wcode,ci,inv;};
struct F55_Star{u64 rot[3];u64 pmask;};
struct F55_SparseLeg{u32 src,dst;u64 rot;};
struct F55_Sparse{int n;F55_SparseLeg l[16];u64 pmask;};
struct F55_Word{int n;u32 v[8];i8 sg[8];};
struct F55_Orbit{int k;u32 off,n;u64 basisMask;};

// Permanent public basis + authoritative current state.
static u32 f55BasisSite[F55_SURFACE];
static F55_Star f55BasisStar[F55_SURFACE];
static u32 f55BasisRankBySite[F55_TOTAL];
static u64 f55OrbitMask[12];
static int f55BasisReady=0;
static u32 f55IdAtSite[F55_TOTAL];
static u32 f55SiteOfId[F55_TOTAL];
static u64 f55OriOfId[F55_TOTAL];
// Scratch and recovered correction plan.
static u64 f55PairQueue[F55_MAX_QUEUE];
