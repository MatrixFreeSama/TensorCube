#!/usr/bin/env python3
from pathlib import Path
import hashlib, re, struct
ROOT=Path(__file__).resolve().parent
EXE=ROOT/'dist'/'TensorCube_11D_Order4.exe'
EXPECTED_EXE_SHA='3822abb2879e90423a907d6b5713afa37b205f2445a1de9d3cadd392c041e848'
EXPECTED_EXE_SIZE=74752
EXPECTED_SECTIONS={
 '.text':'31c856f65671b16519178148d937f015436d39e220c2f5123cd7e219b41417b5',
 '.rdata':'ce8fe2324636603763b593c999547d662a9563223bef8fa0cb135e5fcab76307',
 '.data':'5686b90192f066fb95f4f285e6f1feac3c4df92cd10ba167368f4391f4140bc9',
 '.rsrc':'c3c7150e369e88e22eafc73084488316c96ffae6fa70dc043616faf50563b206',
}
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def fail(s): raise SystemExit(s)
if not EXE.is_file() or EXE.stat().st_size!=EXPECTED_EXE_SIZE or sha(EXE)!=EXPECTED_EXE_SHA: fail('EXE HASH/SIZE FAIL')
# Verify repository manifest.
for line in (ROOT/'MANIFEST.sha256').read_text('utf-8').splitlines():
    if not line.strip(): continue
    h,rel=line.split(None,1); p=ROOT/rel.strip()
    if not p.is_file() or sha(p)!=h: fail(f'MANIFEST FAIL: {rel}')
# Expand quoted includes to audit the actual modular translation unit.
seen=[]
def expand(p):
    p=p.resolve(); seen.append(p)
    out=[]
    for line in p.read_text('utf-8').splitlines(True):
        m=re.match(r'\s*#include\s+"([^"]+)"',line)
        if not m: out.append(line); continue
        q=p.parent/m.group(1)
        if not q.exists(): q=ROOT/m.group(1)
        if not q.exists(): fail(f'INCLUDE MISSING: {m.group(1)}')
        out.append(expand(q))
    return ''.join(out)
text=expand(ROOT/'TensorCube11D4.cpp')
for token in ['TC_SOURCE_FIBERS','chartForRegion','gPlaneMatchings','gPublicMatchingIndex','TC_ACTIVE_STAGES','recoverStateOnlyStage','inferQuarterFromState','stateOnlyFactorPart']:
    if token in text: fail(f'RETIRED TOKEN PRESENT: {token}')
for token in ['f55Scramble','f55SolveStateOnly','f55ExactVerify','f55ScramblePlaneMask','f55PlanPlaneMask','f55ObservedParityMask','f55RecoverQuotientFromParity','f55QuotientAlgebraSelfTest','f55RecoverQuotientFromState','rebuildRenderSnapshot','updateAnimation();render();Sleep(1)','TensorCube · 11维 4阶']:
    if token not in text: fail(f'REQUIRED TOKEN MISSING: {token}')
if 'F55_D=11' not in text or 'F55_SURFACE=F55_TOTAL-F55_INTERIOR' not in text: fail('11D CONSTANT AUDIT FAIL')
# PE section and icon-resource verification.
b=EXE.read_bytes(); pe=struct.unpack_from('<I',b,0x3c)[0]
if b[pe:pe+4]!=b'PE\0\0': fail('PE SIGNATURE FAIL')
fh=pe+4; nsec=struct.unpack_from('<H',b,fh+2)[0]; osz=struct.unpack_from('<H',b,fh+16)[0]; opt=fh+20
if struct.unpack_from('<H',b,opt)[0]!=0x20b: fail('NOT PE32+')
st=opt+osz; secs=[]
for i in range(nsec):
    o=st+i*40; name=b[o:o+8].rstrip(b'\0').decode(errors='ignore'); vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8); secs.append((name,va,max(vs,rs),rp,rs))
for name,va,sz,rp,rs in secs:
    if name in EXPECTED_SECTIONS and hashlib.sha256(b[rp:rp+rs]).hexdigest()!=EXPECTED_SECTIONS[name]: fail(f'SECTION HASH FAIL: {name}')
def ro(rva):
    for _,va,sz,rp,_ in secs:
        if va<=rva<va+sz: return rp+rva-va
    fail('BAD RVA')
rrva,_=struct.unpack_from('<II',b,opt+112+16); base=ro(rrva)
def ents(rel):
    o=base+rel; nn,ni=struct.unpack_from('<HH',b,o+12); return [struct.unpack_from('<II',b,o+16+8*i) for i in range(nn+ni)]
res={}
for n1,t1 in ents(0):
    if n1&0x80000000 or not t1&0x80000000: continue
    typ=n1&0xffff
    for n2,t2 in ents(t1&0x7fffffff):
        if not t2&0x80000000: continue
        for nl,t3 in ents(t2&0x7fffffff):
            if t3&0x80000000: continue
            de=base+(t3&0x7fffffff); rva,sz,_,_=struct.unpack_from('<IIII',b,de); res[(typ,n2&0xffff,nl&0xffff)]=b[ro(rva):ro(rva)+sz]
icon_hash={
 (3,1,1033):'4fdce9a6e99d109ac14fa1379ec77d40691684dcd8dfcda813734dff9337e452',
 (3,2,1033):'2cb51d19864a9b32a673e302a4e3c362521166c250e828af41cf74dfe8312f4b',
 (3,3,1033):'92bf9af70477fe8dc109e54623ce0f5eefa96aabc552d9ced0fd32814db0d12c',
 (3,4,1033):'dfdf8a04bdd6c563bab95fa91d81a9b8bff36ef851ca77265105ea48d7a22787',
 (3,5,1033):'3841ee4943cdd2ef73316628eda5b69ca0e2bf4be7946107f646e3f3e8cf35ab',
 (3,6,1033):'ab3f7c73272f9e25ae178d47d898c4a3871186a11db3ee6653638ecf9406c179',
 (3,7,1033):'8941d39412c71c03a9ce333a2c5f53d171b9aed618855f4936047336d848131c',
 (14,101,1033):'f6a7a0c20ab3ec737f2cba302251f0c4bb5927a5b49332b11377b8bef13c57ac',
}
for k,h in icon_hash.items():
    if k not in res or hashlib.sha256(res[k]).hexdigest()!=h: fail(f'ICON RESOURCE FAIL: {k}')
for marker in [b'CreateFileW',b'CreateFileA',b'ReadFile',b'WriteFile',b'DeleteFileW',b'RegSetValue',b'glFinish']:
    if marker in b: fail(f'FORBIDDEN PE MARKER: {marker.decode()}')
print('PASS: TensorCube 11D Order-4 package/static audit')
print('PASS: modular source expands to current 55-plane state-only architecture')
print('PASS: retired disjoint-plane/fiber solve paths absent')
print('PASS: PE code/data/resource section hashes and source icon resources')
