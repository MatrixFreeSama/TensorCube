static void updateGpuText(){
    wchar_t b[1900];wchar_t*p=b;*p=0;
    p=wappend(p,L"计算设备  ·  ");if(gGL)p=wappendAscii(p,glGetString(GL_RENDERER));else p=wappend(p,L"OpenGL unavailable");
    if(gGL){p=wappend(p,L"  ·  OpenGL ");p=wappendAscii(p,glGetString(GL_VERSION));}
    p=wappend(p,L"  ·  Tensor Core ");p=wappend(p,gCUDA.wmma?L"WMMA ON":L"OFF");
    p=wappend(p,L"  ·  CPU workers ");p=wappendInt(p,gCPUWorkers);
    stableSetText(gGpuText,gGpuCache,1900,b);
}
static void updateSolverLine(){
    wchar_t b[2300];wchar_t*p=b;*p=0;
    if(gSolveUiState==TC_RANDOMIZING){
        p=wappend(p,L"随机状态定义中  ·  ");p=wappendFixed2(p,nowSeconds()-gActivityStart);p=wappend(p,L" s");
    }else if(gSolveUiState==TC_FACTORIZING){
        p=wappend(p,L"精确求解  ·  ");p=wappendFixed2(p,nowSeconds()-gActivityStart);p=wappend(p,L" s");
    }else if(gSolveUiState==TC_PATH_READY){
        p=wappend(p,L"求解完成  ·  Exact ");p=wappend(p,gStats.exact?L"PASS":L"FAIL");
        p=wappend(p,L"  ·  ");p=wappendFixed2(p,gStats.seconds);p=wappend(p,L" s  ·  factors ");p=wappendULL(p,gStats.factors);p=wappend(p,L"  ·  可播放");
    }else if(gSolveUiState==TC_PLAYING){
        unsigned long long done=(unsigned long long)gPlaybackQuotientApplied+(unsigned long long)gPlaybackApplied;
        unsigned long long total=(unsigned long long)gRecoveredQuotientCount+(unsigned long long)f55PlanCount;
        p=wappend(p,L"回放  ·  ");p=wappendULL(p,done);p=wappend(p,L" / ");p=wappendULL(p,total);
        p=wappend(p,L"  ·  目标 ");p=wappendFixed2(p,gPlaybackTargetTotal);p=wappend(p,L" s");
    }else if(gSolveUiState==TC_SOLVED){
        p=wappend(p,L"已回到解态  ·  Exact PASS");
    }else if(gSolveUiState==TC_FAILED){
        p=wappend(p,L"失败  ·  ");p=wappend(p,gSolveFailure);
    }else{
        p=wappend(p,gScrambled?L"随机状态已定义  ·  可开始求解":L"就绪");
    }
    stableSetText(gSolverText,gSolverCache,2300,b);
}
static void updatePaperData(){
    wchar_t b[2400];wchar_t*p=b;*p=0;
    p=wappend(p,L"论文数据 / PAPER DATA\r\nD = 11    N = 4    Surface sites = ");p=wappendULL(p,surfaceSites(gCfg));
    p=wappend(p,L"\r\nOrbit components = 11    Legal slice moves = 14417920");
    if(gLastScrambleSeconds>0){
        p=wappend(p,L"\r\nDeep random = ");p=wappendFixed2(p,gLastScrambleSeconds);p=wappend(p,L" s");
    }
    if(gSolveUiState==TC_PATH_READY||gSolveUiState==TC_PLAYING||gSolveUiState==TC_SOLVED){
        p=wappend(p,L"\r\nSolve time = ");p=wappendFixed2(p,gStats.seconds);p=wappend(p,L" s");
        p=wappend(p,L"\r\nFactors = ");p=wappendULL(p,gStats.factors);p=wappend(p,L"    Exact plan = ");p=wappend(p,gStats.exact?L"PASS":L"FAIL");
    }else if(gScrambled){
        p=wappend(p,L"\r\nState = deep legal tensor");
    }else{
        p=wappend(p,L"\r\nState = solved");
    }
    p=wappend(p,L"\r\nDisplay = 11D projection views");
    stableSetText(gPaperText,gPaperCache,2400,b);
}
static void updateViewPageUi(){
    if(gViewPageText){wchar_t b[256];wchar_t*p=b;*p=0;p=wappend(p,L"视图页 ");p=wappendInt(p,gViewPage+1);p=wappend(p,L"/");p=wappendInt(p,gViewPageCount);stableSetText(gViewPageText,gPageCache,256,b);}
}
static void updateStatus(){
    wchar_t b[1800];wchar_t*p=b;*p=0;
    p=wappend(p,L"TensorCube  ·  11维  ·  4阶");
    p=wappend(p,L"  ·  表面块 ");p=wappendULL(p,surfaceSites(gCfg));
    p=wappend(p,L"  ·  轨道分量 11  ·  合法层旋转 14417920");
    stableSetText(gStatus,gStatusCache,1800,b);
    updateViewPageUi();updateGpuText();updateSolverLine();updatePaperData();
}
static void handleShortcut(UINT key){if(key==VK_ESCAPE){DestroyWindow(gWnd);return;}if(key==VK_PRIOR||key=='Q'){changeViewPage(-1);return;}if(key==VK_NEXT||key=='E'){changeViewPage(1);return;}if(key=='S'){doScramble();return;}if(key=='R'){doSolve();return;}if(key=='P'){doPlay();return;}}
