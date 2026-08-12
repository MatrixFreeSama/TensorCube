// Presentation snapshot: copied from the source shell's separation between
// authoritative solve state and the continuously rendered front view.  The
// worker threads may mutate the solver StateVector, while OpenGL reads only
// this tiny, stable projection cache.  It is refreshed only at committed UI
// boundaries, so deep-random / solve never freeze the message/render loop.
static i8 gRenderFace[5][4][64][6]; static BOOL gRenderSnapshotReady=FALSE;
static int renderPosIndex(int x,int y,int z){return (x*4+y)*4+z;}
static void rebuildRenderSnapshot(){
    for(int page=0;page<gViewPageCount;page++)for(int vi=0;vi<4;vi++){
        const ViewState&v=gPagedViews[page][vi];
        for(int x=0;x<4;x++)for(int y=0;y<4;y++)for(int z=0;z<4;z++){
            u8 c[32];fillCoordForView(v,x,y,z,c);int q=renderPosIndex(x,y,z);
            gRenderFace[page][vi][q][0]=(i8)currentFace(c,{v.axis[0],1});
            gRenderFace[page][vi][q][1]=(i8)currentFace(c,{v.axis[0],-1});
            gRenderFace[page][vi][q][2]=(i8)currentFace(c,{v.axis[1],1});
            gRenderFace[page][vi][q][3]=(i8)currentFace(c,{v.axis[1],-1});
            gRenderFace[page][vi][q][4]=(i8)currentFace(c,{v.axis[2],1});
            gRenderFace[page][vi][q][5]=(i8)currentFace(c,{v.axis[2],-1});
        }
    }
    gRenderSnapshotReady=TRUE;
}
static void focusHiddenSliceOnPlan(u32 zi){
    if(f55PlanCount==0)return;if(zi>=f55PlanCount)zi=f55PlanCount-1;
    u32 q=f55Plan[zi],v=q&0xfffffu,k=(q>>21)&15u;if(k<1||k>11)return;
    F55_Orbit O={(int)k,F55_ORBIT_OFF[k],F55_ORBIT_N[k],f55OrbitMask[k]};
    if(v>=O.n)return;u8 c[32];decodeIndex4(f55BasisSite[O.off+v],c);gHiddenSlice=(u64)encodeIndex4(c);
}
static void draw11DCubie(int viewIndex,const ViewState& v,int x,int y,int z){const float h=0.44f;colorBlack();for(int f=0;f<6;f++)quadFace(f,h);int n=gCfg.n-1,q=renderPosIndex(x,y,z);
    if(x==n){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][0]:20);drawSticker(0);}if(x==0){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][1]:20);drawSticker(1);}
    if(y==n){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][2]:20);drawSticker(2);}if(y==0){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][3]:20);drawSticker(3);}
    if(z==n){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][4]:20);drawSticker(4);}if(z==0){facePaletteColor(gRenderSnapshotReady?gRenderFace[gViewPage][viewIndex][q][5]:20);drawSticker(5);}
}
static float cameraBlurStrength(const ViewState& v){float t=(v.zoom-TC_CAMERA_BLUR_START_Z)/(TC_CAMERA_ZOOM_MAX-TC_CAMERA_BLUR_START_Z);if(t<0)t=0;if(t>1)t=1;return t*t*(3-2*t);} static float cameraRenderZoom(const ViewState&v){return v.zoom>TC_CAMERA_SAFE_Z?TC_CAMERA_SAFE_Z:v.zoom;}
static void drawBlurQuad(float dx,float dy,float alpha){glColor4f(1,1,1,alpha);glBegin(GL_QUADS);glTexCoord2f(0,0);glVertex3f(-1+dx,-1+dy,0);glTexCoord2f(1,0);glVertex3f(1+dx,-1+dy,0);glTexCoord2f(1,1);glVertex3f(1+dx,1+dy,0);glTexCoord2f(0,1);glVertex3f(-1+dx,1+dy,0);glEnd();}
static void drawNearCameraBlur(int vx,int vy,int vw,int vh,float strength){if(strength<=0.001f||vw<8||vh<8)return;if(!gNearBlurTex)glGenTextures(1,&gNearBlurTex);glBindTexture(GL_TEXTURE_2D,gNearBlurTex);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,vx,vy,vw,vh,0);glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glMatrixMode(GL_PROJECTION);glLoadIdentity();glMatrixMode(GL_MODELVIEW);glLoadIdentity();float px=2.0f/(float)vw,py=2.0f/(float)vh,r=1.5f+5.0f*strength,a=0.055f+0.060f*strength;drawBlurQuad(r*px,0,a);drawBlurQuad(-r*px,0,a);drawBlurQuad(0,r*py,a);drawBlurQuad(0,-r*py,a);drawBlurQuad(r*px,r*py,a*.8f);drawBlurQuad(-r*px,r*py,a*.8f);drawBlurQuad(r*px,-r*py,a*.8f);drawBlurQuad(-r*px,-r*py,a*.8f);glDisable(GL_TEXTURE_2D);glColor4f(1,1,1,0.06f+0.18f*strength);glBegin(GL_QUADS);glVertex3f(-1,-1,0);glVertex3f(1,-1,0);glVertex3f(1,1,0);glVertex3f(-1,1,0);glEnd();glDisable(GL_BLEND);glColor3f(1,1,1);glEnable(GL_DEPTH_TEST);}
static void drawViewBorder(int active){glDisable(GL_DEPTH_TEST);glMatrixMode(GL_PROJECTION);glLoadIdentity();glMatrixMode(GL_MODELVIEW);glLoadIdentity();if(active)glColor3f(.23f,.25f,.28f);else glColor3f(.82f,.83f,.85f);float t=active?.015f:.008f;glBegin(GL_QUADS);glVertex3f(-1,-1,0);glVertex3f(1,-1,0);glVertex3f(1,-1+t,0);glVertex3f(-1,-1+t,0);glVertex3f(-1,1-t,0);glVertex3f(1,1-t,0);glVertex3f(1,1,0);glVertex3f(-1,1,0);glVertex3f(-1,-1,0);glVertex3f(-1+t,-1,0);glVertex3f(-1+t,1,0);glVertex3f(-1,1,0);glVertex3f(1-t,-1,0);glVertex3f(1,-1,0);glVertex3f(1,1,0);glVertex3f(1-t,1,0);glEnd();}
static void renderOneView(int idx,const VRect& r){if(r.w<2||r.h<2)return;int vy=gHeight-(r.y+r.h);glViewport(r.x,vy,r.w,r.h);drawStudioShadow();glEnable(GL_DEPTH_TEST);glDepthFunc(GL_LEQUAL);float aspect=(float)r.w/(float)r.h;glMatrixMode(GL_PROJECTION);glLoadIdentity();double ss=.68;glFrustum(-ss*aspect,ss*aspect,-ss,ss,1.2,120.0);glMatrixMode(GL_MODELVIEW);glLoadIdentity();ViewState&v=currentView(idx);glTranslatef(0,-.05f,cameraRenderZoom(v));glRotatef(v.pitch,1,0,0);glRotatef(v.yaw,0,1,0);float center=(float)(gCfg.n-1)*.5f,spacing=.91f;for(int x=0;x<gCfg.n;x++)for(int y=0;y<gCfg.n;y++)for(int z=0;z<gCfg.n;z++){glPushMatrix();glTranslatef(((float)x-center)*spacing,((float)y-center)*spacing,((float)z-center)*spacing);draw11DCubie(idx,v,x,y,z);glPopMatrix();}float blur=cameraBlurStrength(v);if(blur>.001f)drawNearCameraBlur(r.x,vy,r.w,r.h,blur);drawViewBorder(idx==gActiveView);}
static void render(){if(!gGL)return;glViewport(0,0,gWidth,gHeight);glClearColor(1,1,1,1);glClearDepth(1.0);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);VRect r[4];getViewRects(r);for(int i=0;i<4;i++)renderOneView(i,r[i]);glFlush();SwapBuffers(gDC);}

static void setDiag(wchar_t* dst,int cap,const wchar_t* text){int i=0;if(cap<=0)return;while(text&&*text&&i<cap-1)dst[i++]=*text++;dst[i]=0;}
static wchar_t gContextDiag[192]=L"not initialized";
static BOOL badWglProc(void* p){UINT_PTR v=(UINT_PTR)p;return p==0||v==1u||v==2u||v==3u||v==(UINT_PTR)-1;} static void* loadGL(const char* name){void* p=wglGetProcAddress(name);return badWglProc(p)?0:p;}
static HGLRC createBestCompatibilityContext(HDC dc){HGLRC temp=wglCreateContext(dc);if(!temp){setDiag(gContextDiag,192,L"legacy bootstrap context creation failed");return 0;}if(!wglMakeCurrent(dc,temp)){wglDeleteContext(temp);setDiag(gContextDiag,192,L"legacy bootstrap context make-current failed");return 0;}PFNWGLCREATECONTEXTATTRIBSARBPROC createAttrib=(PFNWGLCREATECONTEXTATTRIBSARBPROC)loadGL("wglCreateContextAttribsARB");if(createAttrib){const int versions[][2]={{4,6},{4,5},{4,4},{4,3}};for(int i=0;i<4;i++){int attrs[]={WGL_CONTEXT_MAJOR_VERSION_ARB,versions[i][0],WGL_CONTEXT_MINOR_VERSION_ARB,versions[i][1],WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,0};HGLRC modern=createAttrib(dc,0,attrs);if(modern){wglMakeCurrent(0,0);if(wglMakeCurrent(dc,modern)){wglDeleteContext(temp);wchar_t* p=gContextDiag;*p=0;p=wappend(p,L"explicit OpenGL ");p=wappendInt(p,versions[i][0]);p=wappend(p,L".");p=wappendInt(p,versions[i][1]);p=wappend(p,L" compatibility context");return modern;}wglDeleteContext(modern);wglMakeCurrent(dc,temp);}}}setDiag(gContextDiag,192,L"driver compatibility bootstrap context");return temp;}

// -----------------------------------------------------------------------------
