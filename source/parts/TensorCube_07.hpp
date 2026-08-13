    glColor3f(1,1,1);for(int f=0;f<6;f++)drawHighFaceDirect(f,h+0.006f);
    pglUseProgram(0);
}
static float tcCameraBlurStrength(){
    float t=(gZoom-TC_CAMERA_BLUR_START_Z)/(TC_CAMERA_ZOOM_MAX-TC_CAMERA_BLUR_START_Z);
    if(t<0.0f)t=0.0f;if(t>1.0f)t=1.0f;return t*t*(3.0f-2.0f*t);
}
static float tcCameraRenderZoom(){return gZoom>TC_CAMERA_SAFE_Z?TC_CAMERA_SAFE_Z:gZoom;}
static void drawBlurQuad(float dx,float dy,float alpha){
    glColor4f(1.0f,1.0f,1.0f,alpha);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex3f(-1.0f+dx,-1.0f+dy,0);
    glTexCoord2f(1,0);glVertex3f( 1.0f+dx,-1.0f+dy,0);
    glTexCoord2f(1,1);glVertex3f( 1.0f+dx, 1.0f+dy,0);
    glTexCoord2f(0,1);glVertex3f(-1.0f+dx, 1.0f+dy,0);
    glEnd();
}
static void drawNearCameraBlur(float strength){
    if(strength<=0.001f||gWidth<8||gHeight<8)return;
    if(!gNearBlurTex)glGenTextures(1,&gNearBlurTex);
    glBindTexture(GL_TEXTURE_2D,gNearBlurTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,gWidth,gHeight,0);
    gNearBlurW=gWidth;gNearBlurH=gHeight;

    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glMatrixMode(GL_MODELVIEW);glLoadIdentity();

    float px=2.0f/(float)gWidth,py=2.0f/(float)gHeight;
    float r=(1.5f+5.0f*strength);
    float a=0.055f+0.060f*strength;
    drawBlurQuad( r*px,0,a);drawBlurQuad(-r*px,0,a);
    drawBlurQuad(0, r*py,a);drawBlurQuad(0,-r*py,a);
    drawBlurQuad( r*px, r*py,a*0.80f);drawBlurQuad(-r*px, r*py,a*0.80f);
    drawBlurQuad( r*px,-r*py,a*0.80f);drawBlurQuad(-r*px,-r*py,a*0.80f);

    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,0.06f+0.18f*strength);
    glBegin(GL_QUADS);glVertex3f(-1,-1,0);glVertex3f(1,-1,0);glVertex3f(1,1,0);glVertex3f(-1,1,0);glEnd();
    glDisable(GL_BLEND);glColor3f(1,1,1);glEnable(GL_DEPTH_TEST);
}
static void render(){
    int rh=gHeight;if(rh<100)rh=100;
    glViewport(0,0,gWidth,rh);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClearDepth(1.0);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    drawStudioShadow();
    glEnable(GL_DEPTH_TEST);glDepthFunc(GL_LEQUAL);
    float aspect=(float)gWidth/(float)rh;
    glMatrixMode(GL_PROJECTION);glLoadIdentity();double ss=0.68;
    glFrustum(-ss*aspect,ss*aspect,-ss,ss,1.2,120.0);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glTranslatef(0.0f,-0.05f,tcCameraRenderZoom());glRotatef(gPitch,1,0,0);glRotatef(gYaw,0,1,0);
    if(gOrder<=TC_PHYSICAL_RENDER_MAX){
        float center=(float)(gOrder-1)*0.5f,spacing=0.91f;
        int axis=0,layer=0,dir=1,turns=1;
        if(gSolveUiState==TC_PLAYING&&gCurrentMove>=0)moveInfo(gCurrentMove,&axis,&layer,&dir,&turns);
        for(int i=0;i<gCubieCount;i++){
            Cubie& c=gCubies[i];glPushMatrix();
            if(gSolveUiState==TC_PLAYING&&gCurrentMove>=0&&cubieAffected(c,gCurrentMove)){
                if(axis==0)glRotatef(gCurrentAngle,1,0,0);else if(axis==1)glRotatef(gCurrentAngle,0,1,0);else glRotatef(gCurrentAngle,0,0,1);
            }
            glTranslatef(((float)c.x-center)*spacing,((float)c.y-center)*spacing,((float)c.z-center)*spacing);
            float m[16];m[0]=(float)c.bx.x;m[1]=(float)c.bx.y;m[2]=(float)c.bx.z;m[3]=0;
            m[4]=(float)c.by.x;m[5]=(float)c.by.y;m[6]=(float)c.by.z;m[7]=0;
            m[8]=(float)c.bz.x;m[9]=(float)c.bz.y;m[10]=(float)c.bz.z;m[11]=0;
            m[12]=m[13]=m[14]=0;m[15]=1;glMultMatrixf(m);drawCubieLocal(c);glPopMatrix();
        }
    }else{
        drawHighMatrixFree();
    }
    float nearBlur=tcCameraBlurStrength();if(nearBlur>0.001f)drawNearCameraBlur(nearBlur);
    glFlush();SwapBuffers(gDC);
}
static void setDiag(wchar_t* dst,int cap,const wchar_t* text){int i=0;if(cap<=0)return;while(text&&*text&&i<cap-1)dst[i++]=*text++;dst[i]=0;}
static void setComputeMissing(const char* name){wchar_t* p=gComputeDiag;*p=0;p=wappend(p,L"missing OpenGL entry: ");while(name&&*name&&p<gComputeDiag+2046)*p++=(wchar_t)(unsigned char)*name++;*p=0;}
static BOOL requireComputeEntry(void* p,const char* name){if(p)return TRUE;setComputeMissing(name);return FALSE;}
static BOOL badWglProc(void* p){UINT_PTR v=(UINT_PTR)p;return p==0||v==1u||v==2u||v==3u||v==(UINT_PTR)-1;}
static void* loadGL(const char* name){void* p=wglGetProcAddress(name);return badWglProc(p)?0:p;}
static HGLRC createBestCompatibilityContext(HDC dc){HGLRC temp=wglCreateContext(dc);if(!temp){setDiag(gContextDiag,192,L"legacy bootstrap context creation failed");return 0;}if(!wglMakeCurrent(dc,temp)){wglDeleteContext(temp);setDiag(gContextDiag,192,L"legacy bootstrap context make-current failed");return 0;}PFNWGLCREATECONTEXTATTRIBSARBPROC createAttrib=(PFNWGLCREATECONTEXTATTRIBSARBPROC)loadGL("wglCreateContextAttribsARB");if(createAttrib){const int versions[][2]={{4,6},{4,5},{4,4},{4,3}};for(int i=0;i<4;i++){int attrs[]={WGL_CONTEXT_MAJOR_VERSION_ARB,versions[i][0],WGL_CONTEXT_MINOR_VERSION_ARB,versions[i][1],WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,0};HGLRC modern=createAttrib(dc,0,attrs);if(modern){wglMakeCurrent(0,0);if(wglMakeCurrent(dc,modern)){wglDeleteContext(temp);wchar_t* p=gContextDiag;*p=0;p=wappend(p,L"explicit OpenGL ");p=wappendInt(p,versions[i][0]);p=wappend(p,L".");p=wappendInt(p,versions[i][1]);p=wappend(p,L" compatibility context");return modern;}wglDeleteContext(modern);wglMakeCurrent(dc,temp);}}}setDiag(gContextDiag,192,L"driver compatibility bootstrap context");return temp;}
static void setComputeLog(const wchar_t* prefix,const char* log){wchar_t* p=gComputeDiag;*p=0;p=wappend(p,prefix);if(log){while(*log&&p<gComputeDiag+2046){unsigned char c=(unsigned char)*log++;if(c=='\r')continue;*p++=(wchar_t)c;}}*p=0;}
static GLuint compileComputeProgram(const char* src,const wchar_t* label){
    GLuint sh=pglCreateShader(GL_COMPUTE_SHADER);pglShaderSource(sh,1,&src,0);pglCompileShader(sh);GLint ok=0;pglGetShaderiv(sh,GL_COMPILE_STATUS,&ok);
    if(!ok){GLsizei got=0;gShaderLogBuf[0]=0;pglGetShaderInfoLog(sh,4095,&got,gShaderLogBuf);setComputeLog(label,gShaderLogBuf);return 0;}
    GLuint pr=pglCreateProgram();pglAttachShader(pr,sh);pglLinkProgram(pr);pglGetProgramiv(pr,GL_LINK_STATUS,&ok);
    if(!ok){GLsizei got=0;gShaderLogBuf[0]=0;pglGetProgramInfoLog(pr,4095,&got,gShaderLogBuf);setComputeLog(label,gShaderLogBuf);return 0;}return pr;
}

static void setHighRenderLog(const wchar_t* prefix,const char* log){
    wchar_t* p=gHighRenderDiag;*p=0;p=wappend(p,prefix);
    if(log)while(*log&&p<gHighRenderDiag+1022){unsigned char c=(unsigned char)*log++;if(c!='\r')*p++=(wchar_t)c;}
    *p=0;
}
static GLuint compileRenderProgram(const char* vs,const char* fs){
    GLuint sv=pglCreateShader(GL_VERTEX_SHADER);pglShaderSource(sv,1,&vs,0);pglCompileShader(sv);GLint ok=0;pglGetShaderiv(sv,GL_COMPILE_STATUS,&ok);
    if(!ok){GLsizei got=0;gShaderLogBuf[0]=0;pglGetShaderInfoLog(sv,4095,&got,gShaderLogBuf);setHighRenderLog(L"vertex shader: ",gShaderLogBuf);return 0;}
    GLuint sf=pglCreateShader(GL_FRAGMENT_SHADER);pglShaderSource(sf,1,&fs,0);pglCompileShader(sf);pglGetShaderiv(sf,GL_COMPILE_STATUS,&ok);
    if(!ok){GLsizei got=0;gShaderLogBuf[0]=0;pglGetShaderInfoLog(sf,4095,&got,gShaderLogBuf);setHighRenderLog(L"fragment shader: ",gShaderLogBuf);return 0;}
    GLuint pr=pglCreateProgram();pglAttachShader(pr,sv);pglAttachShader(pr,sf);pglLinkProgram(pr);pglGetProgramiv(pr,GL_LINK_STATUS,&ok);
    if(!ok){GLsizei got=0;gShaderLogBuf[0]=0;pglGetProgramInfoLog(pr,4095,&got,gShaderLogBuf);setHighRenderLog(L"render link: ",gShaderLogBuf);return 0;}
    return pr;
}


static const char* kGpuGenerateShader=R"GLSL(#version 430 core
layout(local_size_x=64) in;
layout(std430,binding=0) readonly buffer TBuf{uint t[];};
layout(std430,binding=1) buffer SBuf{uint state[];};
uniform uint orderN,startId,itemCount,nonce;
uniform uvec4 key0,key1;
const uint TYPES=9u,LEVELS=32u,DEG=144u,WORDS=36u;
const uint OFF_DEG=0u,OFF_BL=9u,OFF_BASE=18u,OFF_CC=306u,OFF_CHOICES=594u,OFF_WLEN=42066u,OFF_TRANS=83538u,OFF_INV=1576530u;
uint rotl32(uint x,uint n){return (x<<n)|(x>>(32u-n));}
void qr(inout uint a,inout uint b,inout uint c,inout uint d){a+=b;d^=a;d=rotl32(d,16u);c+=d;b^=c;b=rotl32(b,12u);a+=b;d^=a;d=rotl32(d,8u);c+=d;b^=c;b=rotl32(b,7u);}
uint kw(uint i){return i<4u?key0[i]:key1[i-4u];}
