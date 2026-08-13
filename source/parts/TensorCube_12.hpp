    const unsigned int OFF_DEG=0u,OFF_BL=9u,OFF_BASE=18u,OFF_CC=306u,OFF_CHOICES=594u,OFF_WLEN=42066u,OFF_TRANS=83538u,OFF_INV=1576530u,TOTAL=3069522u;
    unsigned int* blob=(unsigned int*)valloc((unsigned long long)TOTAL*4ull);if(!blob)return FALSE;memset(blob,0,(unsigned long long)TOTAL*4ull);
    for(unsigned int i=0;i<9u*32u*144u;i++)blob[OFF_WLEN+i]=0xffffffffu;
    for(int type=0;type<9;type++){
        if(type==3){blob[OFF_DEG+type]=36u;blob[OFF_BL+type]=0u;continue;}
        MfFactorTemplate* t=gMfTemplates[type];if(!t)continue;blob[OFF_DEG+type]=(unsigned int)t->degree;blob[OFF_BL+type]=(unsigned int)t->baseLen;
        for(int lev=0;lev<t->baseLen;lev++){
            blob[OFF_BASE+(unsigned int)type*32u+(unsigned int)lev]=(unsigned int)t->base[lev];
            blob[OFF_CC+(unsigned int)type*32u+(unsigned int)lev]=(unsigned int)t->choiceCount[lev];
            for(int k=0;k<(int)t->choiceCount[lev];k++)blob[OFF_CHOICES+((unsigned int)type*32u+(unsigned int)lev)*144u+(unsigned int)k]=(unsigned int)t->choices[lev][k];
            for(int beta=0;beta<t->degree;beta++)if(t->transValid[lev][beta]){
                unsigned int e=((unsigned int)type*32u+(unsigned int)lev)*144u+(unsigned int)beta;blob[OFF_WLEN+e]=(unsigned int)t->transWord[lev][beta].len;SPerm inv;permInv(&inv,&t->trans[lev][beta],t->degree);
                unsigned int base=OFF_TRANS+e*36u,ibase=OFF_INV+e*36u;for(int w=0;w<36;w++){blob[base+(unsigned int)w]=packPermWord(&t->trans[lev][beta],w);blob[ibase+(unsigned int)w]=packPermWord(&inv,w);}
            }
        }
    }
    pglBindBuffer(GL_SHADER_STORAGE_BUFFER,gGpuTemplateBuf);pglBufferData(GL_SHADER_STORAGE_BUFFER,(GLsizeiptr)((unsigned long long)TOTAL*4ull),blob,GL_STATIC_DRAW);vfree(blob);gGpuTemplateOrder=gOrder;return TRUE;
}

static unsigned long long claimReadyWork(unsigned int amount){for(;;){LONGLONG old=gWorkNext;if(old>=gWorkTotal)return (unsigned long long)gWorkTotal;LONGLONG remain=gWorkTotal-old;LONGLONG take=(LONGLONG)amount;if(take>remain)take=remain;LONGLONG prev=tcAtomicCompareExchange64(&gWorkNext,old+take,old);if(prev==old)return (unsigned long long)old;}}
static void resetWorkerStats(){for(int i=0;i<gCpuWorkerCount;i++){gCpuWorkers[i].chunkSize=512u;gCpuWorkers[i].completed=0;gCpuWorkers[i].records=0;gCpuWorkers[i].tokens=0;gCpuWorkers[i].busySeconds=0.0;}}
static void wakeCpuWorkers(WorkerJobKind job){tcAtomicExchange(&gCpuWorkersDone,0);tcAtomicExchange(&gCpuJob,(LONG)job);for(int i=0;i<gCpuWorkerCount;i++)ReleaseSemaphore(gCpuWorkerWake[i],1,0);}

static void cpuRunQuotient(CpuWorker* w){
    unsigned int chunk=64u;int m=gOrder/2;
    for(;;){if(gAbortRequested||gWorkFailed)break;unsigned long long start=claimReadyWork(chunk);if(start>=(unsigned long long)gWorkTotal)break;unsigned long long end=start+(unsigned long long)chunk;if(end>(unsigned long long)gWorkTotal)end=(unsigned long long)gWorkTotal;double t0=nowSeconds();BOOL ok=TRUE;
        for(unsigned long long q=start;q<end;q++){int cls=(int)q;unsigned long long ci;BigComponent c;if(cls<m){ci=(unsigned long long)cls;c=mfMakeComponent(m,m-cls,0);}else{ci=gMfComponentCount-1;c=mfMakeComponent(0,0,0);}SPerm st;if(!mfReadComponentState(ci,c,&st)){ok=FALSE;break;}gMfQuotientClassBits[cls]=(BYTE)(componentParityPerm(c,&st)?1:0);}
        if(!ok){tcAtomicExchange(&gWorkFailed,1);break;}unsigned long long done=end-start;tcAtomicExchangeAdd64(&gWorkCompleted,(LONGLONG)done);w->completed+=done;w->busySeconds+=nowSeconds()-t0;
    }
}
static void cpuRunFactor(CpuWorker* w){
    for(;;){if(gAbortRequested||gWorkFailed)break;unsigned int chunk=w->chunkSize;unsigned long long start=claimReadyWork(chunk);if(start>=(unsigned long long)gWorkTotal)break;unsigned long long end=start+(unsigned long long)chunk;if(end>(unsigned long long)gWorkTotal)end=(unsigned long long)gWorkTotal;MfCursor cur;if(!mfCursorSeek(&cur,start)){tcAtomicExchange(&gWorkFailed,1);break;}double t0=nowSeconds();unsigned long long records=0,tokens=0;BOOL ok=TRUE;
        for(unsigned long long ci=start;ci<end;ci++){BigComponent c;unsigned long long got;if(!mfCursorNext(&cur,&c,&got)||got!=ci){ok=FALSE;break;}CompactFactor f;BOOL nonId=FALSE;SPerm id;if(!mfFactorOne(ci,c,&f,&nonId,&id)){ok=FALSE;break;}if(nonId){records++;tokens+=(unsigned long long)f.tokenCount;}}
        double dt=nowSeconds()-t0;if(!ok){tcAtomicExchange(&gWorkFailed,1);break;}unsigned long long done=end-start;tcAtomicExchangeAdd64(&gWorkCompleted,(LONGLONG)done);tcAtomicExchangeAdd64(&gCpuCompleted,(LONGLONG)done);tcAtomicExchangeAdd64(&gWorkRecords,(LONGLONG)records);tcAtomicExchangeAdd64(&gWorkTokens,(LONGLONG)tokens);w->completed+=done;w->records+=records;w->tokens+=tokens;w->busySeconds+=dt;
        if(dt<0.003&&w->chunkSize<TC_CPU_CHUNK_MAX){unsigned int n=w->chunkSize<<1;w->chunkSize=n>TC_CPU_CHUNK_MAX?TC_CPU_CHUNK_MAX:n;}else if(dt>0.020&&w->chunkSize>TC_CPU_CHUNK_MIN){unsigned int n=w->chunkSize>>1;w->chunkSize=n<TC_CPU_CHUNK_MIN?TC_CPU_CHUNK_MIN:n;}
    }
}
static DWORD __stdcall cpuWorkerEntry(LPVOID p){CpuWorker* w=(CpuWorker*)p;for(;;){WaitForSingleObject(gCpuWorkerWake[w->index],INFINITE);if(gCpuStop)break;LONG job=gCpuJob;if(job==WK_QUOTIENT)cpuRunQuotient(w);else if(job==WK_FACTOR)cpuRunFactor(w);tcAtomicIncrement(&gCpuWorkersDone);}return 0;}
static BOOL initCpuWorkerPool(){DWORD hw=GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);if(hw<2)hw=2;int count=(int)hw-1;if(count<1)count=1;if(count>TC_MAX_CPU_WORKERS)count=TC_MAX_CPU_WORKERS;gCpuWorkerCount=0;gCpuStop=0;for(int i=0;i<count;i++){gCpuWorkerWake[i]=CreateSemaphoreW(0,0,1,0);if(!gCpuWorkerWake[i])break;gCpuWorkers[i].index=i;gCpuWorkers[i].chunkSize=512u;gCpuWorkers[i].thread=CreateThread(0,0,cpuWorkerEntry,&gCpuWorkers[i],0,0);if(!gCpuWorkers[i].thread){CloseHandle(gCpuWorkerWake[i]);gCpuWorkerWake[i]=0;break;}gCpuWorkerCount=i+1;}if(gCpuWorkerCount==count)return TRUE;tcAtomicExchange(&gCpuStop,1);for(int i=0;i<gCpuWorkerCount;i++)if(gCpuWorkerWake[i])ReleaseSemaphore(gCpuWorkerWake[i],1,0);for(int i=0;i<gCpuWorkerCount;i++){if(gCpuWorkers[i].thread){WaitForSingleObject(gCpuWorkers[i].thread,INFINITE);CloseHandle(gCpuWorkers[i].thread);gCpuWorkers[i].thread=0;}if(gCpuWorkerWake[i]){CloseHandle(gCpuWorkerWake[i]);gCpuWorkerWake[i]=0;}}gCpuWorkerCount=0;return FALSE;}
static void shutdownCpuWorkerPool(){if(gCpuWorkerCount<=0)return;tcAtomicExchange(&gCpuStop,1);for(int i=0;i<gCpuWorkerCount;i++)if(gCpuWorkerWake[i])ReleaseSemaphore(gCpuWorkerWake[i],1,0);for(int i=0;i<gCpuWorkerCount;i++){if(gCpuWorkers[i].thread){WaitForSingleObject(gCpuWorkers[i].thread,INFINITE);CloseHandle(gCpuWorkers[i].thread);gCpuWorkers[i].thread=0;}if(gCpuWorkerWake[i]){CloseHandle(gCpuWorkerWake[i]);gCpuWorkerWake[i]=0;}}gCpuWorkerCount=0;}

static BOOL buildQuotientPlanFromPublishedBits(){
    unsigned long long mb=(unsigned long long)(gMfQuotientClassCount+7)/8;gMfPlanQuotientMask=(BYTE*)valloc(mb);if(!gMfPlanQuotientMask)return FALSE;memset(gMfPlanQuotientMask,0,mb);gMfQuotientMoves=(int*)valloc((unsigned long long)gMfQuotientClassCount*sizeof(int));if(!gMfQuotientMoves)return FALSE;gQuotientMoveCount=0;gRandomQuotientBits=0;
    for(int i=0;i<gMfQuotientClassCount;i++)if(gMfQuotientClassBits[i]){mfMaskSet(gMfPlanQuotientMask,i);gMfQuotientMoves[gQuotientMoveCount++]=moveFrom(0,i,-1);gRandomQuotientBits++;}return TRUE;
}

static BOOL gpuDispatchSlot(GpuSlot* s,unsigned int start,unsigned int count){if(!gComputeReady||!count)return FALSE;s->start=start;s->count=count;s->groups=(count+TC_GPU_LOCAL_SIZE-1u)/TC_GPU_LOCAL_SIZE;s->submitTime=nowSeconds();s->busy=TRUE;
    unsigned int k0[4]={load32le(gMfDesc.key+0),load32le(gMfDesc.key+4),load32le(gMfDesc.key+8),load32le(gMfDesc.key+12)};unsigned int k1[4]={load32le(gMfDesc.key+16),load32le(gMfDesc.key+20),load32le(gMfDesc.key+24),load32le(gMfDesc.key+28)};
    pglUseProgram(gGpuGenerateProgram);pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,gGpuTemplateBuf);pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,s->stateBuf);pglUniform1ui(gGenOrder,(unsigned int)gOrder);pglUniform1ui(gGenStart,start);pglUniform1ui(gGenCount,count);pglUniform1ui(gGenNonce,gMfDesc.nonce);pglUniform4ui(gGenKey0,k0[0],k0[1],k0[2],k0[3]);pglUniform4ui(gGenKey1,k1[0],k1[1],k1[2],k1[3]);pglDispatchCompute(s->groups,1,1);pglMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    pglUseProgram(gGpuFactorProgram);pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,gGpuTemplateBuf);pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,s->stateBuf);pglBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,s->resultBuf);pglUniform1ui(gFacOrder,(unsigned int)gOrder);pglUniform1ui(gFacStart,start);pglUniform1ui(gFacCount,count);pglDispatchCompute(s->groups,1,1);pglMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_BUFFER_UPDATE_BARRIER_BIT);s->fence=pglFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);pglUseProgram(0);glFlush();return s->fence?TRUE:FALSE;
}
static BOOL gpuReadSlotResult(GpuSlot* s,unsigned long long* tokens,unsigned long long* records,unsigned long long* bad){
