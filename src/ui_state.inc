// v32 UI/state-machine shell, adapted only at backend call sites.
// -----------------------------------------------------------------------------
enum SolveUiState{TC_IDLE=0,TC_RANDOMIZING=1,TC_FACTORIZING=2,TC_PATH_READY=3,TC_PLAYING=4,TC_SOLVED=5,TC_FAILED=6};
static SolveUiState gSolveUiState=TC_IDLE; static wchar_t gSolveFailure[512]=L""; static HANDLE gSolveThread=0;
static int gPlaybackMode=0; static double gPlaybackTargetTotal=8.0;
static double gActivityStart=0.0,gLastLiveUi=0.0,gLastPlaybackSnapshot=0.0;

static double parseEditDouble(HWND h,double fallback){wchar_t s[64];int n=GetWindowTextW(h,s,63);double v=0,p=.1;BOOL frac=0,any=0;for(int i=0;i<n;i++){if(s[i]==L'.'){if(!frac)frac=1;continue;}if(s[i]>=L'0'&&s[i]<=L'9'){any=1;int d=s[i]-L'0';if(!frac)v=v*10+d;else{v+=d*p;p*=.1;}}}return any&&v>0?v:fallback;}
static void updateGpuText(); static void updateSolverLine(); static void updatePaperData();
static void updateStatus();
static void setButtons(){BOOL locked=(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING);EnableWindow(gOrderCombo,FALSE);EnableWindow(gBtnScramble,!locked);EnableWindow(gBtnReset,!locked);EnableWindow(gBtnSolve,!locked&&gScrambled);EnableWindow(gBtnPlay,!locked&&gSolveUiState==TC_PATH_READY);EnableWindow(gPlaybackModeCombo,!locked);int mode=(int)SendMessageW(gPlaybackModeCombo,CB_GETCURSEL,0,0);EnableWindow(gPlaybackValueEdit,!locked&&mode!=0);}
static void failUi(const wchar_t*msg){gSolveUiState=TC_FAILED;wchar_t*p=gSolveFailure;*p=0;p=wappend(p,msg);setButtons();updateStatus();}
static void doScramble(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING)return;gScrambled=FALSE;gPlanReady=FALSE;gPlaying=FALSE;gPlayProgress=0;gPlaybackApplied=0;gPlaybackQuotientApplied=0;gRecoveredQuotientMask=0;gRecoveredQuotientCount=0;gStats={};gScrambleOk=FALSE;gScrambleWorking=TRUE;gSolveUiState=TC_RANDOMIZING;gActivityStart=nowSeconds();gLastLiveUi=0;setButtons();updateStatus();gScrambleThread=CreateThread(0,0,scrambleThread,0,0,0);if(!gScrambleThread){gScrambleWorking=FALSE;failUi(L"随机状态定义无法启动");}}
static void doReset(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING)return;f55StateIdentity();gHiddenSlice=0;rebuildRenderSnapshot();gScrambled=FALSE;gPlanReady=FALSE;gPlaying=FALSE;gPlayProgress=0;gPlaybackApplied=0;gPlaybackQuotientApplied=0;gRecoveredQuotientMask=0;gRecoveredQuotientCount=0;gStats={};gSolveUiState=TC_IDLE;setButtons();updateStatus();}
static void doSolve(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING||!gScrambled)return;gSolveUiState=TC_FACTORIZING;gSolving=TRUE;gPlanReady=FALSE;gStats={};gActivityStart=nowSeconds();gLastLiveUi=0;setButtons();updateStatus();gSolveThread=CreateThread(0,0,solveThread,0,0,0);if(!gSolveThread){gSolving=FALSE;failUi(L"精确求解无法启动");}}
static void doPlay(){if(gSolveUiState!=TC_PATH_READY||!gPlanReady||!gScrambled)return;int mode=(int)SendMessageW(gPlaybackModeCombo,CB_GETCURSEL,0,0);double val=parseEditDouble(gPlaybackValueEdit,1.0);if(mode==0)gPlaybackTargetTotal=8.0;else if(mode==1)gPlaybackTargetTotal=val;else gPlaybackTargetTotal=8.0/val;if(gPlaybackTargetTotal<.25)gPlaybackTargetTotal=.25;gPlayDuration=gPlaybackTargetTotal;gPlayStart=nowSeconds();gPlayProgress=0;gPlaybackApplied=0;gPlaybackQuotientApplied=0;gLastPlaybackSnapshot=0;gLastLiveUi=0;gPlaying=TRUE;gSolveUiState=TC_PLAYING;setButtons();updateStatus();}
static void updateAnimation(){
    double now=nowSeconds();
    if((gSolveUiState==TC_RANDOMIZING&&gScrambleWorking)||(gSolveUiState==TC_FACTORIZING&&gSolving)){
        if(now-gLastLiveUi>=0.10){gLastLiveUi=now;updateSolverLine();}
    }
    if(gSolveUiState==TC_RANDOMIZING&&!gScrambleWorking){
        if(gScrambleThread){WaitForSingleObject(gScrambleThread,0);CloseHandle(gScrambleThread);gScrambleThread=0;}
        if(gScrambleOk){gScrambled=TRUE;gSolveUiState=TC_IDLE;rebuildRenderSnapshot();}
        else{wchar_t*p=gSolveFailure;*p=0;p=wappend(p,L"随机状态定义失败");gSolveUiState=TC_FAILED;}
        setButtons();updateStatus();
    }
    if(gSolveUiState==TC_FACTORIZING&&!gSolving){
        if(gSolveThread){WaitForSingleObject(gSolveThread,0);CloseHandle(gSolveThread);gSolveThread=0;}
        if(gPlanReady&&gStats.exact){gSolveUiState=TC_PATH_READY;rebuildRenderSnapshot();}
        else{wchar_t*p=gSolveFailure;*p=0;p=wappend(p,L"精确求解失败");gSolveUiState=TC_FAILED;}
        setButtons();updateStatus();
    }
    if(gSolveUiState==TC_PLAYING&&gPlaying){
        double t=(now-gPlayStart)/gPlayDuration;if(t<0)t=0;if(t>1)t=1;gPlayProgress=t;
        u64 total=(u64)gRecoveredQuotientCount+(u64)f55PlanCount;
        u64 target=(u64)(t*(double)total);if(t>=1.0)target=total;
        int qtarget=(int)(target<(u64)gRecoveredQuotientCount?target:(u64)gRecoveredQuotientCount);
        BOOL changed=FALSE;
        while(gPlaybackQuotientApplied<qtarget){int want=gPlaybackQuotientApplied,seen=0;for(int m=9;m>=0;m--)if(gRecoveredQuotientMask&(1u<<m)){if(seen==want){f55ApplyQuotientMove(m,-1);break;}seen++;}gPlaybackQuotientApplied++;changed=TRUE;}
        u32 starget=target>(u64)gRecoveredQuotientCount?(u32)(target-(u64)gRecoveredQuotientCount):0;
        u32 before=gPlaybackApplied;f55ApplyPlanPrefix(starget,&gPlaybackApplied);if(gPlaybackApplied!=before)changed=TRUE;
        // The source UI continuously exposes playback progress.  For 11D we keep
        // the selected 3D projection tied to the currently consumed state factor,
        // so the visible slice evolves throughout playback rather than changing
        // only on the terminal identity frame.
        if(changed&&(now-gLastPlaybackSnapshot>=0.025||t>=1.0)){
            if(gPlaybackApplied>0)focusHiddenSliceOnPlan(gPlaybackApplied-1);
            rebuildRenderSnapshot();gLastPlaybackSnapshot=now;
        }
        if(now-gLastLiveUi>=0.10){gLastLiveUi=now;updateSolverLine();}
        if(t>=1){BOOL id=f55StateIsIdentity()?TRUE:FALSE;gPlaying=FALSE;gHiddenSlice=0;rebuildRenderSnapshot();if(id){gScrambled=FALSE;gSolveUiState=TC_SOLVED;}else{wchar_t*p=gSolveFailure;*p=0;p=wappend(p,L"播放结束但重放流未通过精确计划验证");gSolveUiState=TC_FAILED;}setButtons();updateStatus();}
    }
}
