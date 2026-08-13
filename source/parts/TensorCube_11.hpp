static void mfFreeTemplates(){for(int t=0;t<9;t++){if(gMfTemplates[t]){vfree(gMfTemplates[t]);gMfTemplates[t]=0;}}}
static BOOL mfBuildTemplate(int type){if(type==3)return TRUE;if(gMfTemplates[type])return TRUE;BigComponent c;if(!mfSampleForType(type,&c))return FALSE;int templateDegree=(int)c.size*6;int rc=kRecipeCount[type];if(rc<=0||rc>TC_MAX_TEMPLATE_GENS)return FALSE;for(int i=0;i<rc;i++)generatorPermFor(c,i,&gOrigPerm[i]);if(!bsgsBuild(templateDegree,gOrigPerm,rc))return FALSE;MfFactorTemplate* t=(MfFactorTemplate*)valloc(sizeof(MfFactorTemplate));if(!t)return FALSE;t->degree=gBsgs.degree;t->baseLen=gBsgs.baseLen;for(int i=0;i<SG_MAX_BASE;i++)t->base[i]=(i<gBsgs.baseLen?gBsgs.base[i]:0);for(int lev=0;lev<SG_MAX_BASE;lev++){t->choiceCount[lev]=0;for(int x=0;x<SG_MAX_DEG;x++)t->transValid[lev][x]=0;}for(int lev=0;lev<gBsgs.baseLen;lev++)for(int x=0;x<gBsgs.degree;x++)if(gBsgs.transValid[lev][x]){t->transValid[lev][x]=1;t->trans[lev][x]=gBsgs.trans[lev][x];t->transWord[lev][x]=gBsgs.transWord[lev][x];t->choices[lev][t->choiceCount[lev]++]=(BYTE)x;}gMfTemplates[type]=t;return TRUE;}
static BOOL mfRandomKernelPerm(unsigned long long ci,int type,SPerm* out){if(type==3){permId(out,36);return TRUE;}if(!mfBuildTemplate(type))return FALSE;MfFactorTemplate* t=gMfTemplates[type];permId(out,t->degree);for(int lev=t->baseLen-1;lev>=0;lev--){int nc=(int)t->choiceCount[lev];if(nc<=0)return FALSE;int choice=(int)mfBound(ci,(unsigned int)lev,0x4B45524Eu^(unsigned int)type,(unsigned int)nc);int beta=(int)t->choices[lev][choice];SPerm q;permRmul(&q,&t->trans[lev][beta],out,t->degree);*out=q;}return TRUE;}
static BOOL mfApplyMaskToPerm(const BigComponent& c,SPerm* st,const BYTE* mask,BOOL inverse){int m=gOrder/2,lv[3]={m,(int)c.a,(int)c.b};for(int k=0;k<3;k++){BOOL dup=FALSE;for(int j=0;j<k;j++)if(lv[j]==lv[k])dup=TRUE;if(dup)continue;int cls=m-lv[k];if(cls<0||cls>=gMfQuotientClassCount||!mfMaskGet(mask,cls))continue;if(!applyPrimitiveToPerm(c,st,moveFrom(0,cls,inverse?-1:1)))return FALSE;}return TRUE;}
static BOOL mfApplyGeneratedQuotient(const BigComponent& c,SPerm* st){int m=gOrder/2,lv[3]={m,(int)c.a,(int)c.b};for(int k=0;k<3;k++){BOOL dup=FALSE;for(int j=0;j<k;j++)if(lv[j]==lv[k])dup=TRUE;if(dup)continue;int cls=m-lv[k];if(cls<0||cls>=gMfQuotientClassCount||!mfQuotientBit(cls))continue;if(!applyPrimitiveToPerm(c,st,moveFrom(0,cls,1)))return FALSE;}return TRUE;}
static BOOL mfReadComponentState(unsigned long long ci,const BigComponent& c,SPerm* out){if(!gMfDesc.valid)return FALSE;if(!mfRandomKernelPerm(ci,(int)c.type,out))return FALSE;if(!mfApplyGeneratedQuotient(c,out))return FALSE;return TRUE;}

static BOOL mfChooseCompact(int type,const SPerm* target,CompactFactor* f){if(!mfBuildTemplate(type))return FALSE;MfFactorTemplate* t=gMfTemplates[type];if(!t)return FALSE;SPerm h=*target;f->baseLen=(BYTE)t->baseLen;f->tokenCount=0;for(int i=0;i<SG_MAX_BASE;i++)f->chosen[i]=0;for(int i=0;i<t->baseLen;i++){int beta=h.p[t->base[i]];if(beta<0||beta>=t->degree||!t->transValid[i][beta])return FALSE;f->chosen[i]=(BYTE)beta;if(beta==t->base[i])continue;SPerm invu,nh;permInv(&invu,&t->trans[i][beta],t->degree);permRmul(&nh,&invu,&h,t->degree);h=nh;}if(!permIsId(&h,t->degree))return FALSE;unsigned long long total=0;for(int i=t->baseLen-1;i>=0;i--)total+=(unsigned long long)t->transWord[i][f->chosen[i]].len;if(total>0xffffffffull)return FALSE;f->tokenCount=(unsigned int)total;return TRUE;}
static BOOL mfCompactPerm(int type,const CompactFactor* f,SPerm* out){if(!mfBuildTemplate(type))return FALSE;MfFactorTemplate* t=gMfTemplates[type];if(!t||(int)f->baseLen!=t->baseLen)return FALSE;permId(out,t->degree);for(int i=t->baseLen-1;i>=0;i--){int beta=(int)f->chosen[i];if(beta<0||beta>=t->degree||!t->transValid[i][beta])return FALSE;SPerm q;permRmul(&q,&t->trans[i][beta],out,t->degree);*out=q;}return TRUE;}
static void mfCursorReset(MfCursor* c){c->m=gOrder/2;c->lo=(gOrder&1)?0:1;c->a=c->m;c->b=c->m;c->ch=0;c->cc=1;c->ci=0;c->done=FALSE;}
static BOOL mfCursorNext(MfCursor* cur,BigComponent* out,unsigned long long* id){if(cur->done||cur->a<cur->lo)return FALSE;int cc=(cur->m>cur->a&&cur->a>cur->b&&cur->b>0)?2:1;if(cur->ch>=cc)cur->ch=0;*out=mfMakeComponent(cur->a,cur->b,cur->ch);*id=cur->ci++;cur->ch++;if(cur->ch>=cc){cur->ch=0;cur->b--;if(cur->b<cur->lo){cur->a--;cur->b=cur->a;if(cur->a<cur->lo)cur->done=TRUE;}}return TRUE;}
// Random access into the implicit component enumeration.  Workers therefore claim arbitrary
// ready ranges without inheriting a single sequential cursor.
static BOOL mfComponentFromOrdinal(unsigned long long id,BigComponent* out){
    if(id>=gMfComponentCount)return FALSE;
    unsigned long long m=(unsigned long long)(gOrder/2);BOOL odd=(gOrder&1)?TRUE:FALSE;
    unsigned long long topCount=odd?(m+1ull):m;
    unsigned long long a=0,b=0,ch=0;
    if(id<topCount){a=m;b=m-id;ch=0;}
    else if(odd){
        unsigned long long middle=m*(m-1ull);
        if(id>=topCount+middle){a=0;b=0;ch=0;}
        else{
            unsigned long long lo=1,hi=m-1,found=0,start=0,count=0;
            while(lo<=hi){unsigned long long mid=(lo+hi)>>1;unsigned long long st=topCount+middle-mid*(mid+1ull);unsigned long long ct=2ull*mid;if(id<st)lo=mid+1;else if(id>=st+ct){if(mid==0)break;hi=mid-1;}else{found=mid;start=st;count=ct;break;}}
            if(!found||id>=start+count)return FALSE;a=found;unsigned long long off=id-start;if(off==0){b=a;ch=0;}else if(off==2ull*a-1ull){b=0;ch=0;}else{unsigned long long j=off-1ull;b=a-1ull-(j>>1);ch=j&1ull;}
        }
    }else{
        unsigned long long square=(m-1ull)*(m-1ull);unsigned long long lo=1,hi=m-1,found=0,start=0,count=0;
        while(lo<=hi){unsigned long long mid=(lo+hi)>>1;unsigned long long st=topCount+square-mid*mid;unsigned long long ct=2ull*mid-1ull;if(id<st)lo=mid+1;else if(id>=st+ct){if(mid==0)break;hi=mid-1;}else{found=mid;start=st;count=ct;break;}}
        if(!found||id>=start+count)return FALSE;a=found;unsigned long long off=id-start;if(off==0){b=a;ch=0;}else{unsigned long long j=off-1ull;b=a-1ull-(j>>1);ch=j&1ull;}
    }
    *out=mfMakeComponent((int)a,(int)b,(int)ch);return TRUE;
}
static BOOL mfCursorSeek(MfCursor* c,unsigned long long id){if(id>=gMfComponentCount){mfCursorReset(c);c->done=TRUE;c->ci=gMfComponentCount;return FALSE;}BigComponent x;if(!mfComponentFromOrdinal(id,&x))return FALSE;c->m=gOrder/2;c->lo=(gOrder&1)?0:1;c->a=(int)x.a;c->b=(int)x.b;c->ch=(int)x.chir;c->cc=(c->m>c->a&&c->a>c->b&&c->b>0)?2:1;c->ci=id;c->done=FALSE;return TRUE;}
static void mfReleasePlan(){if(gMfPlanQuotientMask){secureZero(gMfPlanQuotientMask,(unsigned long long)((gMfQuotientClassCount+7)/8));vfree(gMfPlanQuotientMask);gMfPlanQuotientMask=0;}if(gMfQuotientMoves){vfree(gMfQuotientMoves);gMfQuotientMoves=0;}if(gMfQuotientClassBits){secureZero(gMfQuotientClassBits,(unsigned long long)gMfQuotientClassCount);vfree(gMfQuotientClassBits);gMfQuotientClassBits=0;}gQuotientMoveCount=0;}
static void mfClearDescriptor(){mfReleasePlan();secureZero(&gMfDesc,sizeof(gMfDesc));gMfDesc.version=2;gMfDesc.valid=FALSE;gMfHasRandom=FALSE;gMfPathReady=FALSE;gMfProgress=0;gMfFactorRecordCount=0;gRandomQuotientBits=-1;}
static BOOL mfCreateRandomDescriptor(){mfReleasePlan();BYTE seed[36];if(BCryptGenRandom(0,seed,36,BCRYPT_USE_SYSTEM_PREFERRED_RNG)!=0)return FALSE;for(int i=0;i<32;i++)gMfDesc.key[i]=seed[i];gMfDesc.nonce=load32le(seed+32);gMfDesc.version=2;gMfDesc.valid=TRUE;secureZero(seed,sizeof(seed));gMfHasRandom=TRUE;gMfPathReady=FALSE;gMfProgress=0;gMfFactorRecordCount=0;gMacroTokenCount=0;gPrimitiveCount=0;gRandomQuotientBits=-1;gExactPlanPass=FALSE;return TRUE;}
static BOOL mfFactorOne(unsigned long long ci,const BigComponent& c,CompactFactor* out,BOOL* nonIdentity,SPerm* result){SPerm st;if(!mfReadComponentState(ci,c,&st))return FALSE;if(!mfApplyMaskToPerm(c,&st,gMfPlanQuotientMask,TRUE))return FALSE;if(componentParityPerm(c,&st))return FALSE;int d=(int)c.size*6;if(c.type==3){if(!permIsId(&st,d))return FALSE;permId(result,d);*nonIdentity=FALSE;out->baseLen=0;out->tokenCount=0;return TRUE;}if(permIsId(&st,d)){permId(result,d);*nonIdentity=FALSE;out->baseLen=0;out->tokenCount=0;return TRUE;}SPerm corr,check,id;permInv(&corr,&st,d);if(!mfChooseCompact((int)c.type,&corr,out))return FALSE;if(!mfCompactPerm((int)c.type,out,&check)||!permEq(&check,&corr,d))return FALSE;permRmul(&id,&corr,&st,d);if(!permIsId(&id,d))return FALSE;*result=id;*nonIdentity=TRUE;return TRUE;}

static BOOL mfPrepareTemplatesForOrder(){for(int type=0;type<9;type++){if(type==3)continue;BigComponent c;if(mfSampleForType(type,&c)&&!mfBuildTemplate(type))return FALSE;}return TRUE;}

static unsigned int packPermWord(const SPerm* p,int w){int i=w*4;return (unsigned int)p->p[i]|((unsigned int)p->p[i+1]<<8)|((unsigned int)p->p[i+2]<<16)|((unsigned int)p->p[i+3]<<24);}
static BOOL gpuUploadTemplates(){
    if(!gComputeReady)return FALSE;if(gGpuTemplateOrder==gOrder)return TRUE;if(!mfPrepareTemplatesForOrder())return FALSE;
