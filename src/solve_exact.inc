static DWORD __stdcall solveThread(void*){
    double a=nowSeconds();SolveStats st={};u64 ops=0;u16 qmask=0;BOOL qRemoved=FALSE,starsApplied=FALSE;
    BOOL ok=f55RecoverQuotientFromState(&qmask);
    if(ok){gRecoveredQuotientMask=qmask;gRecoveredQuotientCount=f55QuotientCount(qmask);f55ApplyQuotientMask(qmask,TRUE);qRemoved=TRUE;u16 pm=1;if(!f55ObservedParityMask(&pm)||pm!=0)ok=FALSE;}
    if(ok)ok=f55SolveStateOnly(&ops)?TRUE:FALSE;
    if(ok&&f55Pop(f55PlanPlaneMask)!=55)ok=FALSE;
    if(ok)ok=f55ExactVerify()?TRUE:FALSE;
    // Full end-to-end check on the authoritative state: quotient inverse is
    // already applied; apply only the recovered star correction and demand I.
    if(ok){u32 ap=0;f55ApplyPlanPrefix(f55PlanCount,&ap);starsApplied=TRUE;if(ap!=f55PlanCount||!f55StateIsIdentity())ok=FALSE;}
    // Restore the exact pre-solve scrambled state using only the recovered plan.
    if(starsApplied){f55ApplyStarPlanInverseGlobal();starsApplied=FALSE;}
    if(qRemoved){f55ApplyQuotientMask(qmask,FALSE);qRemoved=FALSE;}
    st.seconds=nowSeconds()-a;st.factors=(u64)f55PlanCount+(u64)gRecoveredQuotientCount;st.primitive=0;st.exact=ok;st.workers=1;st.gpu=FALSE;
    secureZero(&qmask,sizeof(qmask));gStats=st;gSolving=FALSE;gPlanReady=st.exact;return 0;
}

static BOOL full55StaticSelfTest(){
    if(F55_D!=11||F55_TOTAL!=4194304u||F55_SURFACE!=4192256u)return FALSE;
    if(kLocalCycleCount!=198)return FALSE;
    if(planeCount(gCfg)!=55||surfaceSites(gCfg)!=4192256ull)return FALSE;
    if(!f55QuotientAlgebraSelfTest())return FALSE;f55InitTables();f55StateIdentity();return TRUE;
}
