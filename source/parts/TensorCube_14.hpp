static BOOL loadNextMfMacroToken(){gPlaybackTokenActive=FALSE;for(;;){if(!gMfPlaybackRecordActive){if(!mfReadNextFactorRecord(&gMfPlaybackRecord))return FALSE;gMfPlaybackRecordActive=TRUE;gPlaybackFactorLevel=(int)gMfPlaybackRecord.baseLen-1;gPlaybackFactorWordIndex=0;}MfFactorTemplate* t=gMfTemplates[gMfPlaybackRecord.type];if(!t)return FALSE;while(gPlaybackFactorLevel>=0){int beta=(int)gMfPlaybackRecord.chosen[gPlaybackFactorLevel];SWord* w=&t->transWord[gPlaybackFactorLevel][beta];if(gPlaybackFactorWordIndex<(int)w->len){short tok=w->tok[gPlaybackFactorWordIndex++];int oi=(tok>0?tok:-tok)-1;if(oi<0||oi>=kRecipeCount[gMfPlaybackRecord.type])return FALSE;BigComponent c={gMfPlaybackRecord.a,gMfPlaybackRecord.b,gMfPlaybackRecord.chir,gMfPlaybackRecord.type,gMfPlaybackRecord.size,0};int word[10];macroWordFor(c,oi,word);if(tok<0){int temp[10];for(int k=0;k<10;k++)temp[k]=inverseMove(word[9-k]);for(int k=0;k<10;k++)word[k]=temp[k];}for(int k=0;k<10;k++)gPlaybackWord[k]=word[k];gPlaybackCurrentToken=tok;gPlaybackSubstep=0;gPlaybackTokenActive=TRUE;return TRUE;}gPlaybackFactorLevel--;gPlaybackFactorWordIndex=0;}gMfPlaybackRecordActive=FALSE;}}
static void rebuildLowCubiesMatrixFree(){if(gOrder>TC_PHYSICAL_RENDER_MAX)return;gCubieCount=0;MfCursor cur;mfCursorReset(&cur);BigComponent c;unsigned long long ci;while(mfCursorNext(&cur,&c,&ci)){SPerm st;if(gMfHasRandom){if(!mfReadComponentState(ci,c,&st))return;}else permId(&st,(int)c.size*6);IVec3 sites[24];int sc=componentSites(c,sites);for(int li=0;li<sc;li++){if(gCubieCount>=512)return;int dx=(int)st.p[li*6+0],dy=(int)st.p[li*6+2],dz=(int)st.p[li*6+4];int qli=dx/6;Cubie& cb=gCubies[gCubieCount];cb.id=gCubieCount;cb.ox=sites[li].x;cb.oy=sites[li].y;cb.oz=sites[li].z;cb.x=sites[qli].x;cb.y=sites[qli].y;cb.z=sites[qli].z;cb.bx=dirVec(dx%6);cb.by=dirVec(dy%6);cb.bz=dirVec(dz%6);gCubieCount++;}}}

static double parsePositiveDouble(HWND edit,double fallback){wchar_t b[64];int n=GetWindowTextW(edit,b,63);if(n<=0)return fallback;double v=0.0,frac=0.1;BOOL dot=FALSE,any=FALSE;for(int i=0;i<n;i++){wchar_t c=b[i];if(c>=L'0'&&c<=L'9'){any=TRUE;if(!dot)v=v*10.0+(double)(c-L'0');else{v+=(double)(c-L'0')*frac;frac*=0.1;}}else if(c==L'.'&&!dot)dot=TRUE;else if(c==L' '||c==L'\t')continue;else return fallback;}return (any&&v>0.0)?v:fallback;}
static double defaultUnitDuration(){double n=(double)(gPrimitiveCount?gPrimitiveCount:1ull),base=(gPrimitiveCount>12000ull?0.045:(gPrimitiveCount>4000ull?0.065:(gPrimitiveCount>1200ull?0.095:0.18)));if(gPrimitiveCount>50000ull){double capped=180.0/n;if(capped<base)base=capped;}if(base<0.0000005)base=0.0000005;return base;}
static void preparePlaybackTiming(){
    LRESULT mode=SendMessageW(gPlaybackModeCombo,CB_GETCURSEL,0,0);gPlaybackMode=(mode<0?0:(int)mode);
    gPlaybackValue=parsePositiveDouble(gPlaybackValueEdit,gPlaybackMode==1?30.0:1.0);
    if(gOrder>TC_PHYSICAL_RENDER_MAX){
        double baseTotal=45.0;
        if(gPlaybackMode==1)gPlaybackTargetTotal=gPlaybackValue;
        else if(gPlaybackMode==2)gPlaybackTargetTotal=baseTotal/gPlaybackValue;
        else gPlaybackTargetTotal=baseTotal;
        if(gPlaybackTargetTotal<2.0)gPlaybackTargetTotal=2.0;
        gPlaybackUnit=gPlaybackTargetTotal/(double)(gPrimitiveCount?gPrimitiveCount:1ull);
    }else{
        double dflt=defaultUnitDuration();
        if(gPlaybackMode==1)gPlaybackUnit=gPlaybackValue/(double)(gPrimitiveCount?gPrimitiveCount:1ull);
        else if(gPlaybackMode==2)gPlaybackUnit=dflt/gPlaybackValue;
        else gPlaybackUnit=dflt;
        if(gPlaybackUnit<0.00000002)gPlaybackUnit=0.00000002;
        gPlaybackTargetTotal=gPlaybackUnit*(double)gPrimitiveCount;
    }
}
static void startPlayback(){
    if(gPrimitiveCount<=0||!gMfPathReady)return;
    preparePlaybackTiming();gPlaybackStart=nowSeconds();gPlaybackPrimitiveDone=0;gPlaybackFactorIndex=0;gPlaybackFactorLevel=-1;
    gPlaybackFactorWordIndex=0;gPlaybackCurrentComp=-1;gPlaybackBsgsType=-1;gPlaybackCurrentToken=0;gPlaybackTokenActive=FALSE;
    gPlaybackSubstep=0;gPlaybackQuotientApplied=FALSE;gCurrentMove=-1;gCurrentAngle=0;mfPlaybackReset();
    
    gSolveUiState=TC_PLAYING;setButtons();updateStatus();
}
static BOOL nextPrimitive(int* outMove){if(gPlaybackPrimitiveDone<(unsigned long long)gQuotientMoveCount){*outMove=gMfQuotientMoves[(int)gPlaybackPrimitiveDone];return TRUE;}if(!gPlaybackTokenActive&&!loadNextMfMacroToken())return FALSE;*outMove=gPlaybackWord[gPlaybackSubstep];return TRUE;}
static void commitPlaybackPrimitive(){int mv=0;if(!nextPrimitive(&mv))return;if(gOrder<=TC_PHYSICAL_RENDER_MAX)commitMoveLow(mv);gPlaybackPrimitiveDone++;if(gPlaybackPrimitiveDone<=(unsigned long long)gQuotientMoveCount)return;if(!gPlaybackTokenActive)return;gPlaybackSubstep++;if(gPlaybackSubstep>=10){gPlaybackTokenActive=FALSE;gPlaybackSubstep=0;}}
static void finishPlayback(){BOOL streamOk=(gPlaybackPrimitiveDone==gPrimitiveCount);gCurrentMove=-1;gCurrentAngle=0;if(streamOk&&gExactPlanPass&&gMfPathReady){gSolveUiState=TC_SOLVED;gMfHasRandom=FALSE;gMfPathReady=FALSE;if(gOrder<=TC_PHYSICAL_RENDER_MAX)rebuildLowCubiesMatrixFree();}else{gSolveUiState=TC_FAILED;wchar_t* p=gSolveFailure;*p=0;p=wappend(p,L"播放结束但矩阵自由重放流未通过精确计划验证");}setButtons();updateStatus();}
static void updateAnimation(){
    if(gSolveUiState!=TC_PLAYING)return;
    double now=nowSeconds();

    // High orders use time-compressed screen-space visualization.  The logical order N is never replaced by a 512-order proxy.
    // The exact factor plan was already
    // produced and validated during Solve; playback does not iterate trillions of primitive
    // records merely to paint the screen.
    if(gOrder>TC_PHYSICAL_RENDER_MAX){
        double total=gPlaybackTargetTotal>0.001?gPlaybackTargetTotal:1.0;
        double p=(now-gPlaybackStart)/total;if(p<0.0)p=0.0;if(p>1.0)p=1.0;
        gPlaybackPrimitiveDone=(unsigned long long)(p*(double)gPrimitiveCount);
        if(gPlaybackPrimitiveDone>gPrimitiveCount)gPlaybackPrimitiveDone=gPrimitiveCount;
        gCurrentMove=-1;gCurrentAngle=0.0f;
        if(p>=1.0){gPlaybackPrimitiveDone=gPrimitiveCount;finishPlayback();return;}
        if(now-gLastProgressUi>=0.10){gLastProgressUi=now;updateSolverLine();}
        return;
    }

    double pos=(now-gPlaybackStart)/gPlaybackUnit;if(pos<0)pos=0;
    unsigned long long complete=(unsigned long long)pos;if(complete>gPrimitiveCount)complete=gPrimitiveCount;
    unsigned int guard=0;
    while(gPlaybackPrimitiveDone<complete&&guard<500000u){
        unsigned long long before=gPlaybackPrimitiveDone;commitPlaybackPrimitive();
        if(gPlaybackPrimitiveDone==before){gSolveUiState=TC_FAILED;wchar_t* p=gSolveFailure;*p=0;p=wappend(p,L"矩阵自由重放流无法继续");setButtons();updateStatus();return;}
        guard++;
    }
    if(gPlaybackPrimitiveDone>=gPrimitiveCount){finishPlayback();return;}
    int mv=0;if(nextPrimitive(&mv)){gCurrentMove=mv;float u=(float)(pos-(double)gPlaybackPrimitiveDone);if(u<0)u=0;if(u>1)u=1;float e=u*u*(3.0f-2.0f*u);int a,l,d,t;moveInfo(mv,&a,&l,&d,&t);gCurrentAngle=90.0f*(float)d*e;}
    if(now-gLastProgressUi>=0.10){gLastProgressUi=now;updateSolverLine();}
}

static void resetStateIdentity(){mfClearDescriptor();gMacroTokenCount=0;gPrimitiveCount=0;gQuotientMoveCount=0;gSolveUiState=TC_IDLE;gExactPlanPass=FALSE;if(gOrder<=TC_PHYSICAL_RENDER_MAX)rebuildLowCubiesMatrixFree();updateStatus();}
static BOOL initOrder(int n){if(!((n>=2&&n<=49)||n==100||n==1000||n==10000||n==100000))return FALSE;freeOrderMemory();gOrder=n;gYaw=-35.0f;gPitch=26.0f;gZoom=-8.4f;gSolveUiState=TC_IDLE;gSolveFailure[0]=0;gMacroTokenCount=0;gPrimitiveCount=0;gQuotientMoveCount=0;gExactPlanPass=FALSE;gFactorStage=0;gFactorElapsedMs=0;gMfComponentCount=theoreticalComponentCount(n);gMfQuotientClassCount=(n+1)/2;gMfHasRandom=FALSE;gMfPathReady=FALSE;if(n<=TC_PHYSICAL_RENDER_MAX)rebuildLowCubiesMatrixFree();else if(gComputeReady&&!gpuUploadTemplates())setDiag(gHighRenderDiag,1024,L"Matrix-Free render template upload failed");return TRUE;}

static void moveNotation(int move,wchar_t* out){int a,l,d,t;moveInfo(move,&a,&l,&d,&t);wchar_t* p=out;*p=0;*p++=(a==0?L'X':(a==1?L'Y':L'Z'));*p++=L'[';p=wappendInt(p,l);p=wappend(p,L"]");if(t==2)p=wappend(p,L"2");else p=wappend(p,d>0?L"+":L"-");}
static wchar_t* wappendULL(wchar_t* p,unsigned long long v){wchar_t tmp[32];int n=0;do{tmp[n++]=(wchar_t)(L'0'+(v%10ull));v/=10ull;}while(v);while(n>0)*p++=tmp[--n];*p=0;return p;}
static wchar_t* wappendFixed2(wchar_t* p,double v){if(v<0){*p++=L'-';v=-v;}unsigned long long whole=(unsigned long long)v;p=wappendULL(p,whole);*p++=L'.';int h=(int)((v-(double)whole)*100.0+0.5);if(h>=100)h=99;*p++=(wchar_t)(L'0'+h/10);*p++=(wchar_t)(L'0'+h%10);*p=0;return p;}
static BOOL tcWstrEq(const wchar_t* a,const wchar_t* b){while(*a&&*b){if(*a++!=*b++)return FALSE;}return *a==*b;}
static void tcCopyW(wchar_t* d,int cap,const wchar_t* s){int i=0;if(cap<=0)return;while(s&&*s&&i<cap-1)d[i++]=*s++;d[i]=0;}
