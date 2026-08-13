static BOOL bsgsBuildTrans(BsgsWorkspace* b){for(int lev=0;lev<b->baseLen;lev++){b->distCount[lev]=0;for(int gi=0;gi<b->genCount;gi++)if(fixesBase(&b->gen[gi],b->base,lev)){if(b->distCount[lev]>=SG_MAX_GENS)return FALSE;b->dist[lev][b->distCount[lev]++]=(short)gi;}const int INF=0x3fffffff;int cost[SG_MAX_DEG];BYTE done[SG_MAX_DEG];for(int x=0;x<b->degree;x++){b->transValid[lev][x]=0;cost[x]=INF;done[x]=0;}int alpha=b->base[lev];b->transValid[lev][alpha]=1;cost[alpha]=0;permId(&b->trans[lev][alpha],b->degree);wordEmpty(&b->transWord[lev][alpha]);for(int step=0;step<b->degree;step++){int x=-1,best=INF;for(int q=0;q<b->degree;q++)if(!done[q]&&b->transValid[lev][q]&&cost[q]<best){best=cost[q];x=q;}if(x<0)break;done[x]=1;for(int di=0;di<b->distCount[lev];di++){int gi=b->dist[lev][di],y=b->gen[gi].p[x],nc=cost[x]+(int)b->word[gi].len;if(nc>=cost[y])continue;SWord nw;if(!wordRmul(&nw,&b->word[gi],&b->transWord[lev][x]))return FALSE;b->transValid[lev][y]=1;cost[y]=nc;permRmul(&b->trans[lev][y],&b->gen[gi],&b->trans[lev][x],b->degree);b->transWord[lev][y]=nw;}}}return TRUE;}
static BOOL bsgsBuild(int degree,const SPerm* orig,int origCount){if(degree<=0||degree>SG_MAX_DEG||origCount<=0||origCount>SG_MAX_GENS)return FALSE;BsgsWorkspace* b=&gBsgs;b->degree=degree;b->origCount=origCount;b->genCount=0;b->baseLen=0;for(int i=0;i<origCount;i++){if(permIsId(&orig[i],degree))continue;b->gen[b->genCount]=orig[i];b->word[b->genCount].len=1;b->word[b->genCount].tok[0]=(short)(i+1);b->genCount++;}if(b->genCount==0)return TRUE;for(int gi=0;gi<b->genCount;gi++)if(fixesBase(&b->gen[gi],b->base,b->baseLen)){int m=firstMoved(&b->gen[gi],degree);if(m>=0){if(b->baseLen>=SG_MAX_BASE)return FALSE;b->base[b->baseLen++]=m;}}for(int guard=0;guard<2048;guard++){if(!bsgsBuildTrans(b))return FALSE;BOOL changed=FALSE;for(int lev=b->baseLen-1;lev>=0&&!changed;lev--)for(int beta=0;beta<degree&&!changed;beta++){if(!b->transValid[lev][beta])continue;for(int di=0;di<b->distCount[lev]&&!changed;di++){int gi=b->dist[lev][di],gb=b->gen[gi].p[beta];SPerm g1,h,iu;SWord wg1,wh,iw;permRmul(&g1,&b->gen[gi],&b->trans[lev][beta],degree);if(permEq(&g1,&b->trans[lev][gb],degree))continue;if(!wordRmul(&wg1,&b->word[gi],&b->transWord[lev][beta]))return FALSE;permInv(&iu,&b->trans[lev][gb],degree);permRmul(&h,&iu,&g1,degree);wordInv(&iw,&b->transWord[lev][gb]);if(!wordRmul(&wh,&iw,&wg1))return FALSE;int failLevel=b->baseLen+1;for(int j=lev+1;j<b->baseLen;j++){int x=h.p[b->base[j]];if(x==b->base[j])continue;if(!b->transValid[j][x]){failLevel=j+1;break;}SPerm invu,newh;SWord invw,neww;permInv(&invu,&b->trans[j][x],degree);permRmul(&newh,&invu,&h,degree);wordInv(&invw,&b->transWord[j][x]);if(!wordRmul(&neww,&invw,&wh))return FALSE;h=newh;wh=neww;}if(permIsId(&h,degree)||genDuplicate(b,&h))continue;if(failLevel>b->baseLen){int m=firstMoved(&h,degree);if(m<0)continue;if(b->baseLen>=SG_MAX_BASE)return FALSE;b->base[b->baseLen++]=m;}if(b->genCount>=SG_MAX_GENS)return FALSE;b->gen[b->genCount]=h;b->word[b->genCount]=wh;b->genCount++;changed=TRUE;}}if(!changed){if(!bsgsBuildTrans(b))return FALSE;return TRUE;}}return FALSE;}


static void freeOrderMemory(){mfClearDescriptor();mfFreeTemplates();gGpuTemplateOrder=0;gMacroTokenCount=0;}
static unsigned long long theoreticalComponentCount(int n){unsigned long long m=(unsigned long long)(n/2);if((n&1)==0)return m*m-m+1ull;return m*m+2ull;}


static BOOL cubieAffected(const Cubie& c,int move){int a,l,d,t;moveInfo(move,&a,&l,&d,&t);int p[3]={c.x,c.y,c.z};return p[a]==l;}
static void commitMoveLow(int move){int a,l,d,t;moveInfo(move,&a,&l,&d,&t);for(int q=0;q<t;q++)for(int i=0;i<gCubieCount;i++){Cubie& c=gCubies[i];int p[3]={c.x,c.y,c.z};if(p[a]!=l)continue;int x=c.x,y=c.y,z=c.z;if(a==0){if(d>0){c.y=gOrder-1-z;c.z=y;}else{c.y=z;c.z=gOrder-1-y;}}else if(a==1){if(d>0){c.x=z;c.z=gOrder-1-x;}else{c.x=gOrder-1-z;c.z=x;}}else{if(d>0){c.x=gOrder-1-y;c.y=x;}else{c.x=y;c.y=gOrder-1-x;}}c.bx=rotateVecQuarter(c.bx,a,d);c.by=rotateVecQuarter(c.by,a,d);c.bz=rotateVecQuarter(c.bz,a,d);}}

static void colorBlack(){glColor3f(0.045f,0.05f,0.06f);} 
static void stickerColor(int face){if(face==0)glColor3f(0.90f,0.08f,0.06f);else if(face==1)glColor3f(1.00f,0.38f,0.03f);else if(face==2)glColor3f(0.96f,0.96f,0.96f);else if(face==3)glColor3f(0.98f,0.82f,0.05f);else if(face==4)glColor3f(0.05f,0.72f,0.22f);else glColor3f(0.04f,0.28f,0.95f);}
static void quadFace(int face,float h){glBegin(GL_QUADS);if(face==0){glVertex3f(h,-h,-h);glVertex3f(h,h,-h);glVertex3f(h,h,h);glVertex3f(h,-h,h);}else if(face==1){glVertex3f(-h,-h,h);glVertex3f(-h,h,h);glVertex3f(-h,h,-h);glVertex3f(-h,-h,-h);}else if(face==2){glVertex3f(-h,h,-h);glVertex3f(-h,h,h);glVertex3f(h,h,h);glVertex3f(h,h,-h);}else if(face==3){glVertex3f(-h,-h,h);glVertex3f(-h,-h,-h);glVertex3f(h,-h,-h);glVertex3f(h,-h,h);}else if(face==4){glVertex3f(-h,-h,h);glVertex3f(h,-h,h);glVertex3f(h,h,h);glVertex3f(-h,h,h);}else{glVertex3f(h,-h,-h);glVertex3f(-h,-h,-h);glVertex3f(-h,h,-h);glVertex3f(h,h,-h);}glEnd();}
// Explicit low-order sticker drawing.
static void drawSticker(int face){const float h=0.451f,s=0.345f;glBegin(GL_QUADS);if(face==0){glVertex3f(h,-s,-s);glVertex3f(h,s,-s);glVertex3f(h,s,s);glVertex3f(h,-s,s);}else if(face==1){glVertex3f(-h,-s,s);glVertex3f(-h,s,s);glVertex3f(-h,s,-s);glVertex3f(-h,-s,-s);}else if(face==2){glVertex3f(-s,h,-s);glVertex3f(-s,h,s);glVertex3f(s,h,s);glVertex3f(s,h,-s);}else if(face==3){glVertex3f(-s,-h,s);glVertex3f(-s,-h,-s);glVertex3f(s,-h,-s);glVertex3f(s,-h,s);}else if(face==4){glVertex3f(-s,-s,h);glVertex3f(s,-s,h);glVertex3f(s,s,h);glVertex3f(-s,s,h);}else{glVertex3f(s,-s,-h);glVertex3f(-s,-s,-h);glVertex3f(-s,s,-h);glVertex3f(s,s,-h);}glEnd();}
static void drawCubieLocal(const Cubie& c){const float h=0.44f;colorBlack();for(int f=0;f<6;f++)quadFace(f,h);if(c.ox==gOrder-1){stickerColor(0);drawSticker(0);}if(c.ox==0){stickerColor(1);drawSticker(1);}if(c.oy==gOrder-1){stickerColor(2);drawSticker(2);}if(c.oy==0){stickerColor(3);drawSticker(3);}if(c.oz==gOrder-1){stickerColor(4);drawSticker(4);}if(c.oz==0){stickerColor(5);drawSticker(5);}}

static double tcWrapPi(double x){const double PI=3.14159265358979323846,T=6.28318530717958647692;while(x>PI)x-=T;while(x<-PI)x+=T;return x;}
static double tcSin(double x){x=tcWrapPi(x);double x2=x*x;return x*(1.0-x2/6.0+x2*x2/120.0-x2*x2*x2/5040.0);}
static double tcCos(double x){x=tcWrapPi(x);double x2=x*x;return 1.0-x2/2.0+x2*x2/24.0-x2*x2*x2/720.0;}
static void drawStudioShadow(){
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.80f,0.81f,0.83f);glVertex3f(0.08f,-0.42f,0.0f);
    const int seg=72;const double PI=3.14159265358979323846;
    for(int i=0;i<=seg;i++){
        double a=(double)i*(2.0*PI/(double)seg);
        glColor3f(1.0f,1.0f,1.0f);
        glVertex3f(0.08f+(float)(0.43*tcCos(a)),-0.42f+(float)(0.095*tcSin(a)),0.0f);
    }
    glEnd();
}
static unsigned int tcKeyWord(int i){
    const BYTE* p=gMfDesc.key+i*4;
    return (unsigned int)p[0]|((unsigned int)p[1]<<8)|((unsigned int)p[2]<<16)|((unsigned int)p[3]<<24);
}
static void drawHighFlatFallback(){
    const float h=2.85f;
    colorBlack();for(int f=0;f<6;f++)quadFace(f,h);
    for(int f=0;f<6;f++){stickerColor(f);quadFace(f,h+0.006f);}
}
static void drawHighFaceDirect(int face,float h){
    pglUniform1i(gHrFace,face);
    glBegin(GL_QUADS);
    if(face==0){glVertex3f(h,-h,-h);glVertex3f(h,h,-h);glVertex3f(h,h,h);glVertex3f(h,-h,h);}
    else if(face==1){glVertex3f(-h,-h,h);glVertex3f(-h,h,h);glVertex3f(-h,h,-h);glVertex3f(-h,-h,-h);}
    else if(face==2){glVertex3f(-h,h,-h);glVertex3f(-h,h,h);glVertex3f(h,h,h);glVertex3f(h,h,-h);}
    else if(face==3){glVertex3f(-h,-h,h);glVertex3f(-h,-h,-h);glVertex3f(h,-h,-h);glVertex3f(h,-h,h);}
    else if(face==4){glVertex3f(-h,-h,h);glVertex3f(h,-h,h);glVertex3f(h,h,h);glVertex3f(-h,h,h);}
    else{glVertex3f(h,-h,-h);glVertex3f(-h,-h,-h);glVertex3f(-h,h,-h);glVertex3f(h,h,-h);}
    glEnd();
}
static void drawHighMatrixFree(){
    const float h=2.85f;
    if(!gHighRenderReady||!gComputeReady||gGpuTemplateOrder!=gOrder){drawHighFlatFallback();return;}
    unsigned int k0[4]={tcKeyWord(0),tcKeyWord(1),tcKeyWord(2),tcKeyWord(3)};
    unsigned int k1[4]={tcKeyWord(4),tcKeyWord(5),tcKeyWord(6),tcKeyWord(7)};
    float play=0.0f;
    if(gSolveUiState==TC_PLAYING&&gPlaybackTargetTotal>0.001){
        double q=(nowSeconds()-gPlaybackStart)/gPlaybackTargetTotal;if(q<0.0)q=0.0;if(q>1.0)q=1.0;play=(float)q;
    }
    pglUseProgram(gHighRenderProgram);
    pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,gGpuTemplateBuf);
    pglUniform1ui(gHrOrder,(unsigned int)gOrder);pglUniform1ui(gHrNonce,gMfDesc.nonce);
    pglUniform4ui(gHrKey0,k0[0],k0[1],k0[2],k0[3]);pglUniform4ui(gHrKey1,k1[0],k1[1],k1[2],k1[3]);
    pglUniform1i(gHrHasRandom,gMfHasRandom?1:0);pglUniform1f(gHrPlay,play);
