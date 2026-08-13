# Reproducibility

This document defines the verification boundary for the archived TensorCube artifact.

## Artifact identity

Windows executable:

```text
dist/TensorCube.exe
```

SHA-256:

```text
720dcbcd0c9c3b03006bd668ab1dd3988847cb1dbed1eaf866ee5990d1ea0545
```

A reproduced experiment should first verify this hash when the archived executable itself is being evaluated.

## Build and runtime boundary

The validated build target is a Windows x86-64 GUI executable. The build audit established a successful freestanding Windows x64 build and the following runtime properties:

- final GLSL compilation/linking occurs on the user's OpenGL runtime;
- OpenGL 4.3+ compatibility is required by the executable's compute path;
- `nvcuda.dll` is not statically imported; the CUDA Driver path is runtime-loaded;
- application task-file APIs are absent from the audited executable path;
- registry mutation APIs are absent from the audited executable path;
- `glFinish` is absent from the audited synchronization path.

Vendor-specific runtime behavior must therefore be measured on the machine under test rather than inferred from the build container.

## Reference physical test machine

The primary physical machine reserved for TensorCube measurements is:

```text
System: ROG 枪神9 Plus 超竞版
CPU: Intel Core Ultra 9 275HX
GPU: NVIDIA GeForce RTX 5080 Laptop GPU
Memory: 32 GB
Storage: 4 TB SSD
Display: 18-inch MiniLED
```

The storage capacity is 4 TB. It must not be reported as 2 TB in TensorCube benchmark records.

The static machine specification does not replace per-session environment capture. Record the Windows version, NVIDIA driver version, active power/performance mode, display resolution, and executable SHA-256 at the time of each measured run.

## Exact-state validation

A benchmark run counts as a successful solve only when the program reports exact completion of the discrete target state. A visual transition to six uniform faces is not sufficient by itself.

For experimental reporting, record at least:

```text
order N
surface-site count
orbit-component count
random-state descriptor/seed information when exposed
solver wall-clock time
primitive-equivalent count
GPU solving state
Tensor Core state
exact verification result
```

Failed or rejected runs must not be silently removed from the sample set.

## Performance protocol

For comparisons with another solver, use the same machine whenever possible. Cross-machine ratios should be labelled as wall-clock observations rather than algorithm-only speedups.

Recommended procedure:

1. Restart the executable and record OS, CPU, GPU, driver, power mode, and display resolution.
2. Perform an unmeasured warm-up sufficient to trigger shader compilation and GPU runtime initialization.
3. Generate legal states without supplying the solver a retained scramble history.
4. Measure solver time separately from playback/rendering time.
5. Run multiple independent states per order.
6. Record failures as failures.
7. Report at least mean, median, P95, and P99 wall-clock solve time.
8. Preserve exact-verification status for every measured run.

For a direct comparison against a table-based or CPU solver, also report initialization/precomputation time and memory separately from online solve time.

## Scaling experiments

The supported order catalogue makes decade-scale experiments possible. Useful checkpoints include:

```text
4
10
49
100
1000
10000
100000
```

Scaling claims should plot or tabulate at least:

```text
N
C(N)
S(N)
solve time
peak process memory
GPU memory if measured
primitive-equivalent count
```

Do not infer complexity from two isolated timings alone. Fit against the structural quantity actually consumed by the algorithm, especially orbit-component count, as well as against `N`.

## Rendering experiments

For `N > 10`, rendering cost is screen-space fragment work over a direct logical-state query. Screen resolution must therefore be reported independently from puzzle order.

A rendering benchmark must not describe a reduced screen sampling grid as a reduced physical/logical puzzle order.

## Claims boundary

The artifact demonstrates one exact Matrix-Free discrete computation architecture. It does not, by itself, establish move-count optimality, uniform sampling over the complete mathematical cube group, or a universal complexity theorem for unrelated Matrix-Free problems.
