# Architecture

TensorCube treats a high-order cube as an exact discrete state system whose logical state need not be globally materialized.

## 1. State authority

For each surface element, the authoritative state is:

```text
(i, s, R)
```

where:

- `i` is stable identity,
- `s` is the current logical site,
- `R` is the full discrete orientation.

The solved condition is therefore a discrete identity condition over identity, site, and orientation, rather than a visual-color-only condition.

## 2. Surface scaling

The number of surface sites of an `N×N×N` cube is

```text
S(N) = N³ - (N-2)³
     = 6N² - 12N + 8.
```

The Matrix-Free decomposition partitions these sites into bounded local orbit-component types. For even `N`, the component count is

```text
C(N) = N²/4 - N/2 + 1.
```

Representative scales:

| Order | Surface sites | Orbit components |
|---:|---:|---:|
| 49 | 13,826 | 578 |
| 10,000 | 599,880,008 | 24,995,001 |
| 100,000 | 59,998,800,008 | 2,499,950,001 |

The important property is not that these logical sites disappear. They still exist mathematically. The representation avoids requiring one explicit global state record for every site at the same time.

## 3. Matrix-Free state provider

A compact RAM-resident random descriptor and a component identifier determine the state of a requested component. The state is regenerated deterministically on demand.

Conceptually:

```text
Descriptor + ComponentID + OrbitType
                ↓
       deterministic PRF state
                ↓
  local legal group element reconstruction
                ↓
     exact component permutation/orientation
```

Random-access evaluation is deterministic: query order does not change the reconstructed state.

## 4. Exact factorization

The same representation path is used across the supported order catalogue.

```text
current Matrix-Free tensor
        ↓
quotient inference / factorization
        ↓
component-local exact factorization
        ↓
compact factor records
        ↓
primitive-equivalent count / playback regeneration
```

The design does not depend on a retained scramble path, Pattern Database, IDA*, two-phase lookup solver, solved-side meet table, or a pre-materialized complete high-order tensor.

The solver is not claimed to minimize move count. Its acceptance criterion is exact completion.

## 5. CPU/GPU execution

Work is exposed as component-local factorization tasks. CPU workers and GPU work slots consume the same logical Ready Work domain. The GPU factorization path is asynchronous and does not use `glFinish` as a global serialization barrier.

The NVIDIA acceleration path runtime-loads the CUDA Driver and contains an INT8 WMMA Tensor Core reduction lane. Production use of that lane is guarded by runtime validation rather than assumed from the executable's presence alone.

## 6. Exact verification

A successful solve is not accepted from a visual approximation. The factorization output is checked against the exact discrete target state.

The verification boundary includes:

```text
state reconstruction
→ factorization
→ exact correction equality
→ exact identity check
→ verified completion
```

A failed structural, capacity, or verification condition is a failure, not a partial-success state.

## 7. Direct Matrix-Free rendering

For orders above 10, the renderer does not build a global face image, six persistent face-state textures, or a CPU face-pixel state cache.

For each visible fragment:

```text
fragment local position
→ actual logical surface site
→ orbit component / chirality / ordinal / local index
→ descriptor-derived local group state
→ reverse quotient action
→ required inverse stabilizer transversals
→ source marker
→ sticker color
```

Only the marker required by the fragment is queried. The renderer does not construct the full local marker permutation per fragment.

The near-camera blur is a framebuffer post-process. It is not a materialized cube-state texture.

## 8. Scope

TensorCube is a discrete combinatorial demonstrator for Matrix-Free representation and execution. It is not presented as a general proof that every computational problem can be reduced to the same structure, nor as a shortest-path Rubik's-cube solver.
