    pglGetShaderInfoLog=(PFNGLGETSHADERINFOLOGPROC)loadGL("glGetShaderInfoLog");if(!requireComputeEntry((void*)pglGetShaderInfoLog,"glGetShaderInfoLog"))return FALSE;
    pglCreateProgram=(PFNGLCREATEPROGRAMPROC)loadGL("glCreateProgram");if(!requireComputeEntry((void*)pglCreateProgram,"glCreateProgram"))return FALSE;
    pglAttachShader=(PFNGLATTACHSHADERPROC)loadGL("glAttachShader");if(!requireComputeEntry((void*)pglAttachShader,"glAttachShader"))return FALSE;
    pglLinkProgram=(PFNGLLINKPROGRAMPROC)loadGL("glLinkProgram");if(!requireComputeEntry((void*)pglLinkProgram,"glLinkProgram"))return FALSE;
    pglGetProgramiv=(PFNGLGETPROGRAMIVPROC)loadGL("glGetProgramiv");if(!requireComputeEntry((void*)pglGetProgramiv,"glGetProgramiv"))return FALSE;
    pglGetProgramInfoLog=(PFNGLGETPROGRAMINFOLOGPROC)loadGL("glGetProgramInfoLog");if(!requireComputeEntry((void*)pglGetProgramInfoLog,"glGetProgramInfoLog"))return FALSE;
    pglUseProgram=(PFNGLUSEPROGRAMPROC)loadGL("glUseProgram");if(!requireComputeEntry((void*)pglUseProgram,"glUseProgram"))return FALSE;
    pglGetUniformLocation=(PFNGLGETUNIFORMLOCATIONPROC)loadGL("glGetUniformLocation");if(!requireComputeEntry((void*)pglGetUniformLocation,"glGetUniformLocation"))return FALSE;
    pglUniform1i=(PFNGLUNIFORM1IPROC)loadGL("glUniform1i");if(!requireComputeEntry((void*)pglUniform1i,"glUniform1i"))return FALSE;
    pglUniform1ui=(PFNGLUNIFORM1UIPROC)loadGL("glUniform1ui");if(!requireComputeEntry((void*)pglUniform1ui,"glUniform1ui"))return FALSE;
    pglUniform1f=(PFNGLUNIFORM1FPROC)loadGL("glUniform1f");if(!requireComputeEntry((void*)pglUniform1f,"glUniform1f"))return FALSE;
    pglUniform4ui=(PFNGLUNIFORM4UIPROC)loadGL("glUniform4ui");if(!requireComputeEntry((void*)pglUniform4ui,"glUniform4ui"))return FALSE;
    pglGenBuffers=(PFNGLGENBUFFERSPROC)loadGL("glGenBuffers");if(!requireComputeEntry((void*)pglGenBuffers,"glGenBuffers"))return FALSE;
    pglBindBuffer=(PFNGLBINDBUFFERPROC)loadGL("glBindBuffer");if(!requireComputeEntry((void*)pglBindBuffer,"glBindBuffer"))return FALSE;
    pglBufferData=(PFNGLBUFFERDATAPROC)loadGL("glBufferData");if(!requireComputeEntry((void*)pglBufferData,"glBufferData"))return FALSE;
    pglBindBufferBase=(PFNGLBINDBUFFERBASEPROC)loadGL("glBindBufferBase");if(!requireComputeEntry((void*)pglBindBufferBase,"glBindBufferBase"))return FALSE;
    pglDispatchCompute=(PFNGLDISPATCHCOMPUTEPROC)loadGL("glDispatchCompute");if(!requireComputeEntry((void*)pglDispatchCompute,"glDispatchCompute"))return FALSE;
    pglMemoryBarrier=(PFNGLMEMORYBARRIERPROC)loadGL("glMemoryBarrier");if(!requireComputeEntry((void*)pglMemoryBarrier,"glMemoryBarrier"))return FALSE;
    pglMapBuffer=(PFNGLMAPBUFFERPROC)loadGL("glMapBuffer");if(!requireComputeEntry((void*)pglMapBuffer,"glMapBuffer"))return FALSE;
    pglUnmapBuffer=(PFNGLUNMAPBUFFERPROC)loadGL("glUnmapBuffer");if(!requireComputeEntry((void*)pglUnmapBuffer,"glUnmapBuffer"))return FALSE;
    pglFenceSync=(PFNGLFENCESYNCPROC)loadGL("glFenceSync");if(!requireComputeEntry((void*)pglFenceSync,"glFenceSync"))return FALSE;
    pglClientWaitSync=(PFNGLCLIENTWAITSYNCPROC)loadGL("glClientWaitSync");if(!requireComputeEntry((void*)pglClientWaitSync,"glClientWaitSync"))return FALSE;
    pglDeleteSync=(PFNGLDELETESYNCPROC)loadGL("glDeleteSync");if(!requireComputeEntry((void*)pglDeleteSync,"glDeleteSync"))return FALSE;
    gGpuGenerateProgram=compileComputeProgram(kGpuGenerateShader,L"GLSL matrix-free state provider compile/link failed:\n");if(!gGpuGenerateProgram)return FALSE;
    gGpuFactorProgram=compileComputeProgram(kGpuFactorShader,L"GLSL exact factorizer compile/link failed:\n");if(!gGpuFactorProgram)return FALSE;
    gGenOrder=pglGetUniformLocation(gGpuGenerateProgram,"orderN");gGenStart=pglGetUniformLocation(gGpuGenerateProgram,"startId");gGenCount=pglGetUniformLocation(gGpuGenerateProgram,"itemCount");gGenNonce=pglGetUniformLocation(gGpuGenerateProgram,"nonce");gGenKey0=pglGetUniformLocation(gGpuGenerateProgram,"key0");gGenKey1=pglGetUniformLocation(gGpuGenerateProgram,"key1");
    gFacOrder=pglGetUniformLocation(gGpuFactorProgram,"orderN");gFacStart=pglGetUniformLocation(gGpuFactorProgram,"startId");gFacCount=pglGetUniformLocation(gGpuFactorProgram,"itemCount");
    GLuint ids[1+TC_GPU_SLOTS*2];pglGenBuffers(1+TC_GPU_SLOTS*2,ids);gGpuTemplateBuf=ids[0];
    unsigned long long stateBytes=(unsigned long long)TC_GPU_BATCH_MAX*TC_GPU_TEMPLATE_WORDS*4ull;unsigned long long resultBytes=((unsigned long long)TC_GPU_BATCH_MAX/TC_GPU_LOCAL_SIZE+2ull)*3ull*4ull;
    for(int i=0;i<TC_GPU_SLOTS;i++){gGpuSlots[i].stateBuf=ids[1+i*2];gGpuSlots[i].resultBuf=ids[2+i*2];gGpuSlots[i].fence=0;gGpuSlots[i].busy=FALSE;pglBindBuffer(GL_SHADER_STORAGE_BUFFER,gGpuSlots[i].stateBuf);pglBufferData(GL_SHADER_STORAGE_BUFFER,(GLsizeiptr)stateBytes,0,GL_DYNAMIC_DRAW);pglBindBuffer(GL_SHADER_STORAGE_BUFFER,gGpuSlots[i].resultBuf);pglBufferData(GL_SHADER_STORAGE_BUFFER,(GLsizeiptr)resultBytes,0,GL_DYNAMIC_DRAW);}
    setDiag(gComputeDiag,2048,L"OpenGL matrix-free state provider + exact component factorizer initialized; asynchronous fences enabled");return TRUE;
}

// ================= v26 matrix-free state provider + work-conserving factorization =================

static void secureZero(void* ptr,unsigned long long n){volatile BYTE* p=(volatile BYTE*)ptr;while(n--)*p++=0;}
static unsigned int rotl32(unsigned int x,int n){return (x<<n)|(x>>(32-n));}
static unsigned int load32le(const BYTE* p){return (unsigned int)p[0]|((unsigned int)p[1]<<8)|((unsigned int)p[2]<<16)|((unsigned int)p[3]<<24);}
static void store32le(BYTE* p,unsigned int v){p[0]=(BYTE)v;p[1]=(BYTE)(v>>8);p[2]=(BYTE)(v>>16);p[3]=(BYTE)(v>>24);}
static void chachaQr(unsigned int& a,unsigned int& b,unsigned int& c,unsigned int& d){a+=b;d^=a;d=rotl32(d,16);c+=d;b^=c;b=rotl32(b,12);a+=b;d^=a;d=rotl32(d,8);c+=d;b^=c;b=rotl32(b,7);}
static void mfChaChaBlock(unsigned long long blockIndex,unsigned int streamId,BYTE out[64]){
    static const unsigned int C[4]={0x61707865u,0x3320646eu,0x79622d32u,0x6b206574u};
    unsigned int s[16],x[16];s[0]=C[0];s[1]=C[1];s[2]=C[2];s[3]=C[3];
    for(int i=0;i<8;i++)s[4+i]=load32le(gMfDesc.key+i*4);
    s[12]=(unsigned int)blockIndex;s[13]=streamId;s[14]=(unsigned int)(blockIndex>>32);s[15]=gMfDesc.nonce;
    for(int i=0;i<16;i++)x[i]=s[i];
    for(int r=0;r<10;r++){
        chachaQr(x[0],x[4],x[8],x[12]);chachaQr(x[1],x[5],x[9],x[13]);chachaQr(x[2],x[6],x[10],x[14]);chachaQr(x[3],x[7],x[11],x[15]);
        chachaQr(x[0],x[5],x[10],x[15]);chachaQr(x[1],x[6],x[11],x[12]);chachaQr(x[2],x[7],x[8],x[13]);chachaQr(x[3],x[4],x[9],x[14]);
    }
    for(int i=0;i<16;i++){x[i]+=s[i];store32le(out+i*4,x[i]);}
    secureZero(x,sizeof(x));secureZero(s,sizeof(s));
}
static unsigned int mfRand32(unsigned long long id,unsigned int lane,unsigned int domain){BYTE b[64];mfChaChaBlock(id,domain^(lane*0x9E3779B9u),b);unsigned int v=load32le(b+((lane&15u)*4u));secureZero(b,sizeof(b));return v;}
static unsigned int mfBound(unsigned long long id,unsigned int lane,unsigned int domain,unsigned int bound){if(bound<=1u)return 0u;unsigned int threshold=(0u-bound)%bound;for(unsigned int attempt=0;;attempt++){unsigned int r=mfRand32(id,lane+attempt*17u,domain);if(r>=threshold)return r%bound;}}
static BOOL mfMaskGet(const BYTE* m,int bit){return (m[bit>>3]&(BYTE)(1u<<(bit&7)))?TRUE:FALSE;}
static void mfMaskSet(BYTE* m,int bit){m[bit>>3]|=(BYTE)(1u<<(bit&7));}
static BOOL mfQuotientBit(int cls){return (mfRand32((unsigned long long)cls,0u,0x514F544Du)&1u)?TRUE:FALSE;}
static int componentParityPerm(const BigComponent& c,const SPerm* st){int n=(int)c.size;BYTE p[TC_MAX_COMPONENT_SIZE],seen[TC_MAX_COMPONENT_SIZE];for(int i=0;i<n;i++){p[i]=(BYTE)(st->p[i*6]/6);seen[i]=0;}int cycles=0;for(int i=0;i<n;i++)if(!seen[i]){cycles++;int j=i;while(!seen[j]){seen[j]=1;j=p[j];}}return (n-cycles)&1;}
static BOOL applyPrimitiveToPerm(const BigComponent& c,SPerm* st,int move){SPerm g,res;if(!primitivePermForComponent(c,move,&g))return FALSE;int d=(int)c.size*6;permRmul(&res,&g,st,d);*st=res;return TRUE;}
static BigComponent mfMakeComponent(int a,int b,int ch){int t=compTypeFor(gOrder,a,b,ch);BigComponent c={(unsigned int)a,(unsigned int)b,(BYTE)ch,(BYTE)t,(BYTE)compSizeForType(t),0};return c;}
static BOOL mfSampleForType(int type,BigComponent* out){int m=gOrder/2;
    if(type==0&&gOrder==2){*out=mfMakeComponent(1,1,0);return TRUE;}
    if(type==1){*out=mfMakeComponent(m,m,0);return TRUE;}
    if(type==2&&(gOrder&1)&&m>=1){*out=mfMakeComponent(m,0,0);return TRUE;}
    if(type==4&&m>=2){*out=mfMakeComponent(m,m-1,0);return TRUE;}
    if(type==5&&m>=2){*out=mfMakeComponent(m-1,m-1,0);return TRUE;}
    if(type==6&&(gOrder&1)&&m>=2){*out=mfMakeComponent(m-1,0,0);return TRUE;}
    if(type==7&&m>=3){*out=mfMakeComponent(m-1,m-2,0);return TRUE;}
    if(type==8&&m>=3){*out=mfMakeComponent(m-1,m-2,1);return TRUE;}
    return FALSE;
}
