uint rand32(uint id,uint lane,uint domain){uint s[16],x[16];s[0]=0x61707865u;s[1]=0x3320646eu;s[2]=0x79622d32u;s[3]=0x6b206574u;for(uint i=0u;i<8u;i++)s[4u+i]=kw(i);s[12]=id;s[13]=domain^(lane*0x9E3779B9u);s[14]=0u;s[15]=nonce;for(uint i=0u;i<16u;i++)x[i]=s[i];for(int r=0;r<10;r++){qr(x[0],x[4],x[8],x[12]);qr(x[1],x[5],x[9],x[13]);qr(x[2],x[6],x[10],x[14]);qr(x[3],x[7],x[11],x[15]);qr(x[0],x[5],x[10],x[15]);qr(x[1],x[6],x[11],x[12]);qr(x[2],x[7],x[8],x[13]);qr(x[3],x[4],x[9],x[14]);}uint j=lane&15u;return x[j]+s[j];}
uint boundv(uint id,uint lane,uint domain,uint b){if(b<=1u)return 0u;uint th=(0u-b)%b;for(uint a=0u;;a++){uint r=rand32(id,lane+a*17u,domain);if(r>=th)return r%b;}}
void ident(out uint p[36]){for(uint j=0u;j<36u;j++){uint b=j*4u;p[j]=b|((b+1u)<<8)|((b+2u)<<16)|((b+3u)<<24);}}
uint getp(in uint p[36],uint i){return (p[i>>2]>>((i&3u)*8u))&255u;}
void setp(inout uint p[36],uint i,uint v){uint q=i>>2,s=(i&3u)*8u,m=255u<<s;p[q]=(p[q]&~m)|((v&255u)<<s);}
uint typeOf(uint n,uint a,uint b,uint ch){uint m=n/2u;if(n==2u)return 0u;if(a==m&&b==m)return 1u;if((n&1u)!=0u&&a==m&&b==0u)return 2u;if((n&1u)!=0u&&a==0u&&b==0u)return 3u;if(a==m&&b<m)return 4u;if(a==b)return 5u;if((n&1u)!=0u&&b==0u)return 6u;return 7u+ch;}
uint sizeOf(uint ty){if(ty==0u||ty==1u)return 8u;if(ty==2u)return 12u;if(ty==3u)return 6u;return 24u;}
void componentFromId(uint id,out uint a,out uint b,out uint ch,out uint ty,out uint sz){uint m=orderN/2u;bool odd=(orderN&1u)!=0u;uint top=odd?m+1u:m;a=0u;b=0u;ch=0u;if(id<top){a=m;b=m-id;}else if(odd){uint middle=m*(m-1u);if(id>=top+middle){a=0u;b=0u;}else{uint lo=1u,hi=m-1u,found=0u,st=0u,ct=0u;while(lo<=hi){uint mid=(lo+hi)>>1;uint x=top+middle-mid*(mid+1u);uint c=2u*mid;if(id<x)lo=mid+1u;else if(id>=x+c){if(mid==0u)break;hi=mid-1u;}else{found=mid;st=x;ct=c;break;}}a=found;uint off=id-st;if(off==0u){b=a;ch=0u;}else if(off==2u*a-1u){b=0u;ch=0u;}else{uint j=off-1u;b=a-1u-(j>>1);ch=j&1u;}}}else{uint sq=(m-1u)*(m-1u),lo=1u,hi=m-1u,found=0u,st=0u,ct=0u;while(lo<=hi){uint mid=(lo+hi)>>1;uint x=top+sq-mid*mid;uint c=2u*mid-1u;if(id<x)lo=mid+1u;else if(id>=x+c){if(mid==0u)break;hi=mid-1u;}else{found=mid;st=x;ct=c;break;}}a=found;uint off=id-st;if(off==0u){b=a;ch=0u;}else{uint j=off-1u;b=a-1u-(j>>1);ch=j&1u;}}ty=typeOf(orderN,a,b,ch);sz=sizeOf(ty);}
uint cc(uint ty,uint lev){return t[OFF_CC+ty*LEVELS+lev];}
uint choice(uint ty,uint lev,uint k){return t[OFF_CHOICES+(ty*LEVELS+lev)*DEG+k];}
uint trb(uint ty,uint lev,uint beta,uint i){uint q=OFF_TRANS+(((ty*LEVELS+lev)*DEG+beta)*WORDS)+(i>>2);return (t[q]>>((i&3u)*8u))&255u;}
void main(){uint gid=gl_GlobalInvocationID.x;if(gid>=itemCount)return;uint id=startId+gid,a,b,ch,ty,sz;componentFromId(id,a,b,ch,ty,sz);uint p[36],q[36];ident(p);if(ty!=3u){uint bl=t[OFF_BL+ty];for(int lev=int(bl)-1;lev>=0;lev--){uint n=cc(ty,uint(lev));if(n==0u)break;uint k=boundv(id,uint(lev),0x4B45524Eu^ty,n);uint beta=choice(ty,uint(lev),k);ident(q);uint d=sz*6u;for(uint i=0u;i<d;i++)setp(q,i,trb(ty,uint(lev),beta,getp(p,i)));for(uint j=0u;j<36u;j++)p[j]=q[j];}}uint o=gid*36u;for(uint j=0u;j<36u;j++)state[o+j]=p[j];}
)GLSL";

static const char* kGpuFactorShader=R"GLSL(#version 430 core
layout(local_size_x=64) in;
layout(std430,binding=0) readonly buffer TBuf{uint t[];};
layout(std430,binding=1) readonly buffer SBuf{uint state[];};
layout(std430,binding=2) buffer RBuf{uint result[];};
uniform uint orderN,startId,itemCount;
const uint LEVELS=32u,DEG=144u,WORDS=36u;
const uint OFF_DEG=0u,OFF_BL=9u,OFF_BASE=18u,OFF_CC=306u,OFF_CHOICES=594u,OFF_WLEN=42066u,OFF_TRANS=83538u,OFF_INV=1576530u;
shared uint sTok[64],sRec[64],sBad[64];
void ident(out uint p[36]){for(uint j=0u;j<36u;j++){uint b=j*4u;p[j]=b|((b+1u)<<8)|((b+2u)<<16)|((b+3u)<<24);}}
uint getp(in uint p[36],uint i){return (p[i>>2]>>((i&3u)*8u))&255u;}
void setp(inout uint p[36],uint i,uint v){uint q=i>>2,s=(i&3u)*8u,m=255u<<s;p[q]=(p[q]&~m)|((v&255u)<<s);}
bool isid(in uint p[36],uint d){for(uint i=0u;i<d;i++)if(getp(p,i)!=i)return false;return true;}
uint typeOf(uint n,uint a,uint b,uint ch){uint m=n/2u;if(n==2u)return 0u;if(a==m&&b==m)return 1u;if((n&1u)!=0u&&a==m&&b==0u)return 2u;if((n&1u)!=0u&&a==0u&&b==0u)return 3u;if(a==m&&b<m)return 4u;if(a==b)return 5u;if((n&1u)!=0u&&b==0u)return 6u;return 7u+ch;}
uint sizeOf(uint ty){if(ty==0u||ty==1u)return 8u;if(ty==2u)return 12u;if(ty==3u)return 6u;return 24u;}
void componentFromId(uint id,out uint a,out uint b,out uint ch,out uint ty,out uint sz){uint m=orderN/2u;bool odd=(orderN&1u)!=0u;uint top=odd?m+1u:m;a=0u;b=0u;ch=0u;if(id<top){a=m;b=m-id;}else if(odd){uint middle=m*(m-1u);if(id>=top+middle){a=0u;b=0u;}else{uint lo=1u,hi=m-1u,found=0u,st=0u,ct=0u;while(lo<=hi){uint mid=(lo+hi)>>1;uint x=top+middle-mid*(mid+1u);uint c=2u*mid;if(id<x)lo=mid+1u;else if(id>=x+c){if(mid==0u)break;hi=mid-1u;}else{found=mid;st=x;ct=c;break;}}a=found;uint off=id-st;if(off==0u){b=a;ch=0u;}else if(off==2u*a-1u){b=0u;ch=0u;}else{uint j=off-1u;b=a-1u-(j>>1);ch=j&1u;}}}else{uint sq=(m-1u)*(m-1u),lo=1u,hi=m-1u,found=0u,st=0u,ct=0u;while(lo<=hi){uint mid=(lo+hi)>>1;uint x=top+sq-mid*mid;uint c=2u*mid-1u;if(id<x)lo=mid+1u;else if(id>=x+c){if(mid==0u)break;hi=mid-1u;}else{found=mid;st=x;ct=c;break;}}a=found;uint off=id-st;if(off==0u){b=a;ch=0u;}else{uint j=off-1u;b=a-1u-(j>>1);ch=j&1u;}}ty=typeOf(orderN,a,b,ch);sz=sizeOf(ty);}
uint basev(uint ty,uint lev){return t[OFF_BASE+ty*LEVELS+lev];}
uint wlen(uint ty,uint lev,uint beta){return t[OFF_WLEN+(ty*LEVELS+lev)*DEG+beta];}
uint trb(uint off,uint ty,uint lev,uint beta,uint i){uint q=off+(((ty*LEVELS+lev)*DEG+beta)*WORDS)+(i>>2);return (t[q]>>((i&3u)*8u))&255u;}
void main(){uint lid=gl_LocalInvocationID.x,gid=gl_GlobalInvocationID.x;uint tok=0u,rec=0u,bad=0u;if(gid<itemCount){uint id=startId+gid,a,b,ch,ty,sz;componentFromId(id,a,b,ch,ty,sz);uint d=sz*6u;uint st[36],corr[36],h[36],nh[36],check[36];uint chosen[32];uint o=gid*36u;for(uint j=0u;j<36u;j++)st[j]=state[o+j];bool nonid=!isid(st,d);rec=nonid?1u:0u;if(ty==3u){if(nonid)bad=1u;}else if(nonid){ident(corr);for(uint i=0u;i<d;i++)setp(corr,getp(st,i),i);for(uint j=0u;j<36u;j++)h[j]=corr[j];uint bl=t[OFF_BL+ty];for(uint lev=0u;lev<bl&&bad==0u;lev++){uint beta=getp(h,basev(ty,lev));chosen[lev]=beta;uint wl=wlen(ty,lev,beta);if(wl==0xffffffffu){bad=1u;break;}tok+=wl;if(beta!=basev(ty,lev)){ident(nh);for(uint i=0u;i<d;i++)setp(nh,i,trb(OFF_INV,ty,lev,beta,getp(h,i)));for(uint j=0u;j<36u;j++)h[j]=nh[j];}}if(bad==0u&&!isid(h,d))bad=1u;if(bad==0u){ident(check);for(int lev=int(bl)-1;lev>=0;lev--){uint beta=chosen[uint(lev)];ident(nh);for(uint i=0u;i<d;i++)setp(nh,i,trb(OFF_TRANS,ty,uint(lev),beta,getp(check,i)));for(uint j=0u;j<36u;j++)check[j]=nh[j];}for(uint i=0u;i<d;i++){if(getp(check,i)!=getp(corr,i)){bad=1u;break;}}if(bad==0u)for(uint i=0u;i<d;i++){if(getp(check,getp(st,i))!=i){bad=1u;break;}}}}}sTok[lid]=tok;sRec[lid]=rec;sBad[lid]=bad;barrier();for(uint stride=32u;stride>0u;stride>>=1u){if(lid<stride){sTok[lid]+=sTok[lid+stride];sRec[lid]+=sRec[lid+stride];sBad[lid]+=sBad[lid+stride];}barrier();}if(lid==0u){uint q=gl_WorkGroupID.x*3u;result[q]=sTok[0];result[q+1u]=sRec[0];result[q+2u]=sBad[0];}}
)GLSL";

static const char* kHighRenderVertex=R"GLSL(#version 430 compatibility
out vec3 vLocal;
void main(){vLocal=gl_Vertex.xyz;gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;}
)GLSL";
static const char* kHighRenderFragment=R"GLSL(#version 430 compatibility
layout(std430,binding=0) readonly buffer TBuf{uint t[];};
in vec3 vLocal;
uniform uint orderN,nonce;
uniform uvec4 key0,key1;
uniform int hasRandom,faceId;
uniform float playP;
const uint LEVELS=32u,DEG=144u,WORDS=36u;
const uint OFF_BL=9u,OFF_CC=306u,OFF_CHOICES=594u,OFF_INV=1576530u;
const ivec3 REX[24]=ivec3[24](ivec3(1,0,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,1,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(0,0,-1),ivec3(0,0,1),ivec3(-1,0,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(0,0,1),ivec3(-1,0,0),ivec3(0,0,1),ivec3(0,-1,0),ivec3(0,-1,0),ivec3(0,1,0),ivec3(0,0,1),ivec3(0,-1,0),ivec3(0,-1,0));
const ivec3 REY[24]=ivec3[24](ivec3(0,1,0),ivec3(0,0,1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(0,-1,0),ivec3(1,0,0),ivec3(0,0,1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(-1,0,0),ivec3(0,-1,0),ivec3(0,0,-1),ivec3(0,-1,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,-1,0),ivec3(0,0,1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,0,1));
const ivec3 REZ[24]=ivec3[24](ivec3(0,0,1),ivec3(0,-1,0),ivec3(1,0,0),ivec3(0,0,1),ivec3(0,0,-1),ivec3(0,-1,0),ivec3(1,0,0),ivec3(0,0,-1),ivec3(0,1,0),ivec3(0,-1,0),ivec3(0,0,1),ivec3(0,1,0),ivec3(-1,0,0),ivec3(0,0,-1),ivec3(0,-1,0),ivec3(1,0,0),ivec3(0,1,0),ivec3(-1,0,0),ivec3(0,0,-1),ivec3(0,0,1),ivec3(-1,0,0),ivec3(0,1,0),ivec3(1,0,0),ivec3(-1,0,0));
