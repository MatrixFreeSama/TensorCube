static void stableSetText(HWND h,wchar_t* cache,int cap,const wchar_t* text){if(!h||tcWstrEq(cache,text))return;tcCopyW(cache,cap,text);SetWindowTextW(h,text);}
static void updateGpuText(){
    wchar_t b[1900];wchar_t* p=b;*p=0;
    p=wappend(p,L"计算设备  ·  ");p=wappendAscii(p,glGetString(GL_RENDERER));
    p=wappend(p,L"  ·  OpenGL ");p=wappendAscii(p,glGetString(GL_VERSION));
    p=wappend(p,L"  ·  GPU 求解 ");p=wappend(p,gComputeReady?L"ON":L"OFF");
    p=wappend(p,L"  ·  Tensor Core ");p=wappend(p,gTensorCoreReady?(gTensorCoreReductionActive?L"WMMA ON":L"READY"):L"OFF");
    if(!gTensorCoreReady){p=wappend(p,L" (");p=wappend(p,gTensorCoreDiag);p=wappend(p,L")");}
    p=wappend(p,L"  ·  Render MF ");p=wappend(p,gHighRenderReady?L"ON":L"OFF");
    p=wappend(p,L"  ·  CPU workers ");p=wappendInt(p,gCpuWorkerCount);
    if(!gComputeReady){p=wappend(p,L"  ·  ");p=wappend(p,gComputeDiag);}
    stableSetText(gGpuText,gGpuCache,1900,b);
}
static void updateSolverLine(){
    wchar_t b[2300];wchar_t* p=b;*p=0;
    if(gSolveUiState==TC_RANDOMIZING){
        p=wappend(p,L"随机状态定义中  ·  Matrix-Free  ·  任务磁盘写入 0 B");
    }else if(gSolveUiState==TC_FACTORIZING){
        unsigned long long total=gFactorStage==1?(unsigned long long)gMfQuotientClassCount:gMfComponentCount;
        p=wappend(p,gFactorStage==1?L"关系归约":L"精确求解");
        p=wappend(p,L"  ·  ");p=wappendULL(p,gMfProgress);p=wappend(p,L" / ");p=wappendULL(p,total);
        if(gFactorStage==2){
            p=wappend(p,L"  ·  CPU ");p=wappendULL(p,(unsigned long long)gCpuCompleted);
            p=wappend(p,L"  ·  GPU ");p=wappendULL(p,gGpuCompleted);
            double sec=gFactorElapsedMs*0.001;if(sec>0.0){p=wappend(p,L"  ·  ");p=wappendFixed2(p,((double)gMfProgress/sec)/1000000.0);p=wappend(p,L" M comp/s");}
        }
        p=wappend(p,L"  ·  ");p=wappendFixed2(p,gFactorElapsedMs*0.001);p=wappend(p,L" s");
    }else if(gSolveUiState==TC_PATH_READY){
        p=wappend(p,L"求解完成  ·  Exact ");p=wappend(p,gExactPlanPass?L"PASS":L"FAIL");
        p=wappend(p,L"  ·  ");p=wappendFixed2(p,gFactorElapsedMs*0.001);p=wappend(p,L" s");
        p=wappend(p,L"  ·  primitive ");p=wappendULL(p,gPrimitiveCount);p=wappend(p,L"  ·  可播放");
    }else if(gSolveUiState==TC_PLAYING){
        p=wappend(p,gOrder>TC_PHYSICAL_RENDER_MAX?L"高阶压缩回放  ·  ":L"回放  ·  ");
        p=wappendULL(p,gPlaybackPrimitiveDone+1);p=wappend(p,L" / ");p=wappendULL(p,gPrimitiveCount);
        if(gCurrentMove>=0){wchar_t mv[32];moveNotation(gCurrentMove,mv);p=wappend(p,L"  ·  ");p=wappend(p,mv);}
        p=wappend(p,L"  ·  目标 ");p=wappendFixed2(p,gPlaybackTargetTotal);p=wappend(p,L" s");
    }else if(gSolveUiState==TC_SOLVED){
        p=wappend(p,L"已回到解态  ·  Exact PASS  ·  Matrix-Free");
    }else if(gSolveUiState==TC_FAILED){
        p=wappend(p,L"失败  ·  ");p=wappend(p,gSolveFailure);
    }else{
        p=wappend(p,gMfHasRandom?L"随机状态已定义  ·  可开始求解":L"就绪  ·  Matrix-Free 状态按需生成  ·  任务磁盘写入 0 B");
    }
    stableSetText(gSolverText,gSolverCache,2300,b);
}
static void updatePaperData(){
    wchar_t b[2400];wchar_t* p=b;*p=0;
    p=wappend(p,L"论文数据 / PAPER DATA\r\nN = ");p=wappendInt(p,gOrder);
    p=wappend(p,L"    Surface sites = ");p=wappendULL(p,shellPieceCount(gOrder));
    p=wappend(p,L"\r\nOrbit components = ");p=wappendULL(p,theoreticalComponentCount(gOrder));
    p=wappend(p,L"    Legal slice moves = ");p=wappendInt(p,moveCountForOrder(gOrder));
    if(gSolveUiState==TC_PATH_READY||gSolveUiState==TC_SOLVED||gSolveUiState==TC_PLAYING){
        double sec=gFactorElapsedMs*0.001;
        p=wappend(p,L"\r\nSolve time = ");p=wappendFixed2(p,sec);p=wappend(p,L" s");
        if(sec>0.0){p=wappend(p,L"    Throughput = ");p=wappendFixed2(p,((double)gMfComponentCount/sec)/1000000.0);p=wappend(p,L" M comp/s");}
        p=wappend(p,L"\r\nCPU / GPU completed = ");p=wappendULL(p,(unsigned long long)gCpuCompleted);p=wappend(p,L" / ");p=wappendULL(p,gGpuCompleted);
        p=wappend(p,L"\r\nFactors = ");p=wappendULL(p,gMfFactorRecordCount);p=wappend(p,L"    Macro tokens = ");p=wappendULL(p,gMacroTokenCount);
        p=wappend(p,L"\r\nPrimitive moves = ");p=wappendULL(p,gPrimitiveCount);p=wappend(p,L"    Exact plan = ");p=wappend(p,gExactPlanPass?L"PASS":L"FAIL");
    }else if(gMfHasRandom){
        p=wappend(p,L"\r\nState = deep legal implicit tensor    Materialized tensor = 0 B");
    }else{
        p=wappend(p,L"\r\nState = solved    Backend = Matrix-Free");
    }
    p=wappend(p,L"\r\nDisplay = direct Matrix-Free fragments  /  no face-state texture");
    p=wappend(p,L"\r\nTensorCube task disk I/O = 0 B");
    stableSetText(gPaperText,gPaperCache,2400,b);
}
static void updateStatus(){
    wchar_t b[1800];wchar_t* p=b;*p=0;
    p=wappend(p,L"TensorCube  ·  ");p=wappendInt(p,gOrder);p=wappend(p,L"阶");
    p=wappend(p,L"  ·  表面块 ");p=wappendULL(p,shellPieceCount(gOrder));
    p=wappend(p,L"  ·  轨道分量 ");p=wappendULL(p,theoreticalComponentCount(gOrder));
    p=wappend(p,L"  ·  合法层旋转 ");p=wappendInt(p,moveCountForOrder(gOrder));
    p=wappend(p,L"  ·  Matrix-Free");
    stableSetText(gStatus,gStatusCache,1800,b);
    updateSolverLine();updatePaperData();
}
static void setButtons(){BOOL locked=(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING);EnableWindow(gOrderCombo,!locked);EnableWindow(gBtnScramble,!locked);EnableWindow(gBtnReset,!locked);EnableWindow(gBtnSolve,!locked&&gMfHasRandom);EnableWindow(gBtnPlay,!locked&&gSolveUiState==TC_PATH_READY&&gPrimitiveCount>0);EnableWindow(gPlaybackModeCombo,!locked);LRESULT mode=SendMessageW(gPlaybackModeCombo,CB_GETCURSEL,0,0);EnableWindow(gPlaybackValueEdit,!locked&&mode!=0);}
static void pumpUi(){MSG m;while(PeekMessageW(&m,0,0,0,PM_REMOVE)){if(m.message==0x0012){gAbortRequested=TRUE;PostQuitMessage(0);break;}TranslateMessage(&m);DispatchMessageW(&m);}if(!gAbortRequested)UpdateWindow(gWnd);}

static void doScramble(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING)return;gSolveFailure[0]=0;gSolveUiState=TC_RANDOMIZING;gMacroTokenCount=0;gPrimitiveCount=0;gExactPlanPass=FALSE;gFactorStart=nowSeconds();setButtons();updateStatus();BOOL ok=mfCreateRandomDescriptor();gFactorElapsedMs=(nowSeconds()-gFactorStart)*1000.0;if(ok){if(gOrder<=TC_PHYSICAL_RENDER_MAX)rebuildLowCubiesMatrixFree();}if(!ok){gSolveUiState=TC_FAILED;wchar_t* p=gSolveFailure;*p=0;p=wappend(p,L"matrix-free random descriptor generation failed");}else gSolveUiState=TC_IDLE;setButtons();updateStatus();}
static void doSolve(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING||!gMfHasRandom)return;gSolveFailure[0]=0;gSolveUiState=TC_FACTORIZING;gMacroTokenCount=0;gPrimitiveCount=0;gExactPlanPass=FALSE;setButtons();updateStatus();if(!mfStartExactFactorCurrentTensor()){gSolveUiState=TC_FAILED;wchar_t* p=gSolveFailure;*p=0;p=wappend(p,L"resource-return factorization could not start");setButtons();updateStatus();}}
static void doReset(){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING)return;resetStateIdentity();setButtons();}

static void doPlay(){if(gSolveUiState==TC_PATH_READY&&gPrimitiveCount>0)startPlayback();}
static void selectOrder(int n){if(gSolveUiState==TC_RANDOMIZING||gSolveUiState==TC_FACTORIZING||gSolveUiState==TC_PLAYING)return;if(!initOrder(n)){gSolveUiState=TC_FAILED;wchar_t* p=gSolveFailure;*p=0;p=wappend(p,L"order initialization failed");}setButtons();updateStatus();}
static int orderFromComboIndex(int idx){if(idx>=0&&idx<48)return idx+2;if(idx==48)return 100;if(idx==49)return 1000;if(idx==50)return 10000;if(idx==51)return 100000;return 3;}
static void handleShortcut(UINT key){if(key==VK_ESCAPE){DestroyWindow(gWnd);return;}if(key>='2'&&key<='9'){selectOrder((int)(key-'0'));SendMessageW(gOrderCombo,CB_SETCURSEL,(WPARAM)(key-'2'),0);return;}if(key=='S'){doScramble();return;}if(key=='R'){doSolve();return;}if(key=='P'){doPlay();return;}}

static LRESULT __stdcall OrderListWndProc(HWND h,UINT msg,WPARAM w,LPARAM l){if(msg==WM_MOUSEWHEEL){short delta=(short)((w>>16)&0xFFFF);int steps=(int)delta/120;if(steps==0)steps=delta>0?1:-1;int top=(int)SendMessageW(h,LB_GETTOPINDEX,0,0);int count=(int)SendMessageW(h,LB_GETCOUNT,0,0);top-=steps*3;if(top<0)top=0;if(count>0&&top>count-1)top=count-1;SendMessageW(h,LB_SETTOPINDEX,(WPARAM)top,0);return 0;}return gOrderListOldProc?CallWindowProcW(gOrderListOldProc,h,msg,w,l):DefWindowProcW(h,msg,w,l);}
