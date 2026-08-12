// Rendering reads the same authoritative state used by the factorizer.
// Playback mutates that state only by applying the recovered inverse factors.
static int currentFace(const u8*coord,Normal n){
    u32 site=encodeIndex4(coord);u32 id=f55IdAtSite[site];if(id>=F55_TOTAL)return 20;
    u8 solved[32];decodeIndex4(id,solved);int sa=0,ss=0;
    if(!f55InverseNormalAxis(f55OriOfId[id],n.axis,n.sign,&sa,&ss))return 20;
    Normal src={sa,(i8)ss};int f=solvedFace(gCfg,solved,src);return f<0?20:f;
}

static float palette[24][3]={{1,.15,.10},{1,.55,.05},{1,.9,.1},{.2,.8,.2},{.05,.65,.55},{.05,.55,1},{.15,.25,1},{.55,.2,1},{.9,.15,.85},{1,.2,.55},{.65,.1,.1},{.75,.35,.1},{.75,.7,.08},{.1,.55,.15},{.05,.5,.4},{.05,.4,.75},{.1,.15,.75},{.45,.15,.75},{.72,.1,.62},{.75,.12,.38},{.92,.92,.92},{.15,.15,.15},{.55,.55,.55},{.35,.2,.1}};

// -----------------------------------------------------------------------------
// v32 PRESENTATION SHELL + THIN 11D VIEW ADAPTER
// -----------------------------------------------------------------------------
static void colorBlack(){glColor3f(0.045f,0.05f,0.06f);} 
static void facePaletteColor(int f){if(f<0)f=20;f%=24;glColor3f(palette[f][0],palette[f][1],palette[f][2]);}
static void quadFace(int face,float h){glBegin(GL_QUADS);if(face==0){glVertex3f(h,-h,-h);glVertex3f(h,h,-h);glVertex3f(h,h,h);glVertex3f(h,-h,h);}else if(face==1){glVertex3f(-h,-h,h);glVertex3f(-h,h,h);glVertex3f(-h,h,-h);glVertex3f(-h,-h,-h);}else if(face==2){glVertex3f(-h,h,-h);glVertex3f(-h,h,h);glVertex3f(h,h,h);glVertex3f(h,h,-h);}else if(face==3){glVertex3f(-h,-h,h);glVertex3f(-h,-h,-h);glVertex3f(h,-h,-h);glVertex3f(h,-h,h);}else if(face==4){glVertex3f(-h,-h,h);glVertex3f(h,-h,h);glVertex3f(h,h,h);glVertex3f(-h,h,h);}else{glVertex3f(h,-h,-h);glVertex3f(-h,-h,-h);glVertex3f(-h,h,-h);glVertex3f(h,h,-h);}glEnd();}
static void drawSticker(int face){const float h=0.451f,s=0.345f;glBegin(GL_QUADS);if(face==0){glVertex3f(h,-s,-s);glVertex3f(h,s,-s);glVertex3f(h,s,s);glVertex3f(h,-s,s);}else if(face==1){glVertex3f(-h,-s,s);glVertex3f(-h,s,s);glVertex3f(-h,s,-s);glVertex3f(-h,-s,-s);}else if(face==2){glVertex3f(-s,h,-s);glVertex3f(-s,h,s);glVertex3f(s,h,s);glVertex3f(s,h,-s);}else if(face==3){glVertex3f(-s,-h,s);glVertex3f(-s,-h,-s);glVertex3f(s,-h,-s);glVertex3f(s,-h,s);}else if(face==4){glVertex3f(-s,-s,h);glVertex3f(s,-s,h);glVertex3f(s,s,h);glVertex3f(-s,s,h);}else{glVertex3f(s,-s,-h);glVertex3f(-s,-s,-h);glVertex3f(-s,s,-h);glVertex3f(s,s,-h);}glEnd();}
static double tcWrapPi(double x){const double PI=3.14159265358979323846,T=6.28318530717958647692;while(x>PI)x-=T;while(x<-PI)x+=T;return x;}
static double tcSin(double x){x=tcWrapPi(x);double x2=x*x;return x*(1.0-x2/6.0+x2*x2/120.0-x2*x2*x2/5040.0);} static double tcCos(double x){x=tcWrapPi(x);double x2=x*x;return 1.0-x2/2.0+x2*x2/24.0-x2*x2*x2/720.0;}
static void drawStudioShadow(){glDisable(GL_DEPTH_TEST);glMatrixMode(GL_PROJECTION);glLoadIdentity();glMatrixMode(GL_MODELVIEW);glLoadIdentity();glBegin(GL_TRIANGLE_FAN);glColor3f(0.80f,0.81f,0.83f);glVertex3f(0.08f,-0.42f,0.0f);const int seg=72;const double PI=3.14159265358979323846;for(int i=0;i<=seg;i++){double a=(double)i*(2.0*PI/(double)seg);glColor3f(1,1,1);glVertex3f(0.08f+(float)(0.43*tcCos(a)),-0.42f+(float)(0.095*tcSin(a)),0);}glEnd();}

struct ViewState{int axis[3];float yaw,pitch,zoom;};
// Five pages, four physical viewports. The first 19 triples form an exact
// C(11,3,2)=19 pair-cover of all 55 coordinate planes; the final slot repeats
// X1/X2/X3 as the persistent overview.
static const int gViewPageCount=5;
static ViewState gPagedViews[gViewPageCount][4]={
    {{{0,1,2},-35.0f,26.0f,-8.4f},{{3,4,5},-35.0f,26.0f,-8.4f},{{6,7,8},-35.0f,26.0f,-8.4f},{{0,9,10},-35.0f,26.0f,-8.4f}},
    {{{0,3,7},-35.0f,26.0f,-8.4f},{{0,4,8},-35.0f,26.0f,-8.4f},{{0,5,6},-35.0f,26.0f,-8.4f},{{1,3,6},-35.0f,26.0f,-8.4f}},
    {{{1,4,5},-35.0f,26.0f,-8.4f},{{1,7,10},-35.0f,26.0f,-8.4f},{{1,8,9},-35.0f,26.0f,-8.4f},{{2,3,9},-35.0f,26.0f,-8.4f}},
    {{{2,4,7},-35.0f,26.0f,-8.4f},{{2,5,8},-35.0f,26.0f,-8.4f},{{2,6,10},-35.0f,26.0f,-8.4f},{{3,8,10},-35.0f,26.0f,-8.4f}},
    {{{4,5,10},-35.0f,26.0f,-8.4f},{{4,6,9},-35.0f,26.0f,-8.4f},{{5,7,9},-35.0f,26.0f,-8.4f},{{0,1,2},-35.0f,26.0f,-8.4f}}
};
static int gViewPage=0;
static int gActiveView=0; static BOOL gDragging=FALSE; static int gLastX=0,gLastY=0; static unsigned int gNearBlurTex=0;
static ViewState& currentView(int idx){return gPagedViews[gViewPage][idx];}
static wchar_t* appendViewAxes(wchar_t* p,const ViewState& v){p=wappend(p,L"X");p=wappendInt(p,v.axis[0]+1);p=wappend(p,L"/X");p=wappendInt(p,v.axis[1]+1);p=wappend(p,L"/X");p=wappendInt(p,v.axis[2]+1);return p;}
static BOOL viewCoverageSelfTest(){
    BOOL covered[11][11];memset(covered,0,sizeof(covered));
    // Ignore only the last repeated overview slot.
    for(int page=0;page<gViewPageCount;page++)for(int vi=0;vi<4;vi++){
        if(page==4&&vi==3)continue;
        const ViewState&v=gPagedViews[page][vi];
        for(int a=0;a<3;a++)for(int b=a+1;b<3;b++){int x=v.axis[a],y=v.axis[b];if(x>y){int t=x;x=y;y=t;}covered[x][y]=TRUE;}
    }
    int count=0;for(int a=0;a<11;a++)for(int b=a+1;b<11;b++)if(covered[a][b])count++;
    return count==55;
}
static void updateViewPageUi();
static void updateStatus();
static void changeViewPage(int delta){gViewPage=(gViewPage+delta)%gViewPageCount;if(gViewPage<0)gViewPage+=gViewPageCount;gActiveView=0;updateViewPageUi();updateStatus();}
static const float TC_CAMERA_BLUR_START_Z=-7.15f,TC_CAMERA_SAFE_Z=-6.35f,TC_CAMERA_ZOOM_MAX=-4.20f;
struct VRect{int x,y,w,h;};
static void getViewRects(VRect r[4]){
    int gap=8;if(gWidth<500||gHeight<360){int hw=(gWidth-gap)/2,hh=(gHeight-gap)/2;r[0]={0,0,hw,hh};r[1]={hw+gap,0,gWidth-hw-gap,hh};r[2]={0,hh+gap,hw,gHeight-hh-gap};r[3]={hw+gap,hh+gap,gWidth-hw-gap,gHeight-hh-gap};return;}
    int left=(gWidth*68)/100; if(left<360)left=360;if(left>gWidth-260)left=gWidth-260;int rw=gWidth-left-gap;int h1=(gHeight-2*gap)/3;
    r[0]={0,0,left,gHeight};r[1]={left+gap,0,rw,h1};r[2]={left+gap,h1+gap,rw,h1};r[3]={left+gap,2*(h1+gap),rw,gHeight-2*(h1+gap)};
}
static int hitView(int mx,int my){VRect r[4];getViewRects(r);for(int i=0;i<4;i++)if(mx>=r[i].x&&mx<r[i].x+r[i].w&&my>=r[i].y&&my<r[i].y+r[i].h)return i;return 0;}
static void fillCoordForView(const ViewState& v,int x,int y,int z,u8 c[32]){u64 h=gHiddenSlice;for(int ax=0;ax<gCfg.d;ax++){c[ax]=(u8)(h%(u64)gCfg.n);h/=(u64)gCfg.n;}c[v.axis[0]]=(u8)x;c[v.axis[1]]=(u8)y;c[v.axis[2]]=(u8)z;}

