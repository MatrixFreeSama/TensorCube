<p align="center">
  <img src="assets/icon/TensorCube.png" width="144" alt="TensorCube i/s/R tensor icon">
</p>

# TensorCube 11D Order-4

**GPU Tensor Iteration Solver**

This branch is the standalone `(d, N) = (11, 4)` TensorCube research artifact. It remains separate from `main` and does not redefine the archived main TensorCube executable.

## Fixed mathematical instance

- Dimension: `11`
- Order: `4`
- Full lattice: `4^11 = 4,194,304`
- Interior lattice: `2^11 = 2,048`
- Surface identities: `4,192,256`
- Coordinate rotation planes: `55`
- Layers per plane: `4^9 = 262,144`
- Addressable legal layer descriptors: `14,417,920`
- Natural boundary-count surface orbits: `11`

The authoritative puzzle state is stable identity / logical site / discrete orientation (`i / s / R`). The current state is stored explicitly as StateVector arrays; legal operators and the public group basis are Matrix-Free / sparse.

## 11D legal basis

The source order-4 local legal macro family is lifted onto arbitrary 11D axis triples. A sparse legal star basis is built for every boundary-count surface orbit, and every orbit basis covers all `55` coordinate planes.

The active source contains none of the retired benchmark substitutions: no five-stage disjoint-plane normal form, no public eleven-matching solve family, no `74,752` independent 3D-fiber direct product, no `chartForRegion` / `TC_SOURCE_FIBERS` path, and no reverse-stage quarter-turn inference special case.

## Deep random generation

Every Deep Random request obtains fresh Windows `BCryptGenRandom` material: a `256`-bit key plus a `32`-bit nonce. A ChaCha-style stream drives unbiased Fisher-Yates sampling of an even permutation for every full 11D boundary-count orbit. A separate `10`-bit legal quotient extension supplies the implemented reachable orbit-parity class.

The first Deep Random request can be much slower because it also constructs the reusable public 11D legal basis. Later requests reuse that basis but still obtain fresh random source material and generate a new puzzle state.

After state publication, the generator key, nonce, PRNG state, temporary shuffle and generated quotient bits are not solver inputs. No scramble move history is retained.

## State-derived solve

The solve path reads only the authoritative current StateVector and public legal basis. It derives current orbit parity, recovers the `10`-bit legal quotient by GF(2) elimination, removes that quotient, reconstructs each current orbit permutation, factors it through the sparse legal star basis, builds the legal inverse correction plan, independently replays that plan, requires exact identity, confirms identity on the authoritative state, and restores the exact pre-solve scrambled state using the recovered factors.

`Exact PASS` requires all `4,192,256` surface identities and every complete signed eleven-axis orientation to return exactly to identity.

The current factorization path is CPU state-only. CUDA remains for device reporting and the independent WMMA/Tensor-Core self-test. No GPU/Tensor-Core factorization timing claim is made.

## Timing boundary

Displayed Solve time includes state-derived quotient recovery, state-only factorization, independent full Exact replay, authoritative identity confirmation and restoration of the pre-solve scrambled state. It excludes one-time public-basis construction and Deep Random generation.

### Observed reference solve-time range

Repeated wall-clock observations on the same reference test machine used for the main TensorCube measurements gave:

```text
11D order-4 solve time: 3.9–4.1 s
```

This is an observed range rather than a mean/median/P95/P99 statistic. Because the current 11D order-4 factorization path is CPU state-only, this result must not be presented as GPU or Tensor-Core factorization performance.

## Presentation and 11D observation

The interface fixes the visible mathematical identity to `11维 · 4阶` and retains the mainline TensorCube interaction conventions.

### Visualization limitation: apparent center-first restoration is a projection artifact

The authoritative state is eleven-dimensional, while the interface can show only three-dimensional slices or projections. The other eight coordinates are fixed by the observation chart, so a site that visually resembles a face-center piece is not a semantic "center piece" in the solver.

Projected center-like, edge-like and corner-like locations can belong to different high boundary-count 11D orbits. Replaying exact factors in orbit/factor order can therefore make a projected center-like region appear aligned first. This is an observation-layer limitation of the 11D-to-3D map, not evidence that TensorCube introduces, improves, hybridizes or inherits a conventional higher-order Rubik's-cube strategy.

The solver does not use center solving, edge pairing, `3×3×3` reduction, layer-by-layer solving, CFOP, Roux, ZZ, Kociemba/two-phase search, IDA*, pattern databases, OLL/PLL, or conventional parity-case handling. Changing hidden-axis slices can change which projected region appears to align first, as expected for a projection artifact.

## Executable

```text
dist/TensorCube_11D_Order4.exe
```

SHA-256: `3822abb2879e90423a907d6b5713afa37b205f2445a1de9d3cadd392c041e848`  
Size: `74,752 bytes`.

The embedded `RT_ICON` resources `1..7` and `RT_GROUP_ICON` resource `101` match the approved no-version-suffix source TensorCube icon resources byte-for-byte.

## Source representation

The Windows translation unit is split into small auditable modules under `src/`. `TensorCube11D4.cpp`, `full55_core_freestanding.hpp` and `full55_local_cycles.inc` are thin include-order roots.

A local rebuild of this modular representation produced `.text`, `.rdata`, `.data` and `.rsrc` sections byte-identical to the published executable. Separate whole-file PE hashes differ only through the COFF timestamp field, so the published executable hash above remains the fixed artifact identity.

Run `python validate_11d4.py` from the branch root for the package/static audit.

## Academic scope boundary

The proven family is the implemented canonical source-macro signed-orientation lift plus the state-derived legal parity quotient. This branch does not claim arbitrary pure orientation-kernel state coverage, uniform sampling of every element of the complete abstract 11D puzzle group, that the complete current StateVector itself is Matrix-Free, or GPU/Tensor-Core factorization timing.

## License and authorship

Human investigator: **Zining Kestis Wong** (`MatrixFreeSama`).

This branch is distributed under the **MatrixFreeSama Permissive License 2.0 (MFSPL 2.0)**. By default it imposes no attribution, license-notice retention, reporting, source-publication, or provenance-disclosure requirement. A Licensee who actually copied or adapted Covered Material can later acquire a narrow provenance/project-credit obligation only after receiving a direct valid `Provenance Notice` from the Licensor.

Pure ideas, algorithms, mathematical similarity, inspiration, and genuinely independent implementations that do not reproduce or adapt protected expression are expressly outside the license scope.

See `LICENSE`, `LICENSE_GUIDE.md`, and `AUTHORS.md`.
