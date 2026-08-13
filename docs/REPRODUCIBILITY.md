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

## Reference physical test platform

The primary machine reserved for TensorCube measurements is a 2025 **ROG Strix SCAR 18 (G835LW)**, sold in China as **ROG 枪神9 Plus 超竞版**. The reference unit is recorded as:

```text
System: ROG Strix SCAR 18 (2025) G835LW
CPU: Intel Core Ultra 9 275HX
     24 cores / 24 threads
     up to 5.4 GHz
GPU: NVIDIA GeForce RTX 5080 Laptop GPU
     16 GB GDDR7
     ROG Boost configuration up to 175 W
Memory: 32 GB DDR5 system memory (installed test configuration)
Storage: 4 TB SSD total capacity (installed test configuration)
Display: 18-inch 2560×1600 240 Hz Mini LED ROG Nebula HDR display
```

CPU, GPU, VRAM, rated graphics-power configuration, and display characteristics above follow the manufacturer's G835LW platform specification. The 32 GB memory and 4 TB storage values identify the installed physical test unit and must not be generalized to every G835LW retail configuration.

## ROG operating and GPU modes

ASUS Armoury Crate exposes two separate control families that must not be conflated in benchmark records.

The **Operating Mode** controls system performance, power, cooling and acoustics. For ROG notebooks, ASUS defines `Turbo` as the charging-only mode that maximizes CPU/GPU power for high-performance workloads and uses maximum fan airflow.

The **GPU Mode** controls graphics routing and power behavior. ASUS documents the four modes as follows:

| GPU mode | Official behavior relevant to benchmarking |
|---|---|
| `Ultimate` | Uses the discrete-GPU MUX path for the best performance and lowest button-to-pixel latency. The discrete GPU can bypass the hybrid graphics path and drive the display directly. Higher power use is expected, and a reboot may be required when manually switching the MUX state. |
| `Standard` | Windows-default `MSHybrid`. Demanding applications can use the discrete GPU while non-intensive tasks use the integrated GPU; graphics-heavy frames may be routed through the integrated-graphics display path. |
| `Eco` | Completely disables the discrete GPU for maximum energy saving, lower temperatures and lower noise. This mode is not suitable for TensorCube GPU benchmarking. |
| `Optimized` | ROG automatic policy. On AC power it behaves as the hybrid `Standard` path for demanding versus light workloads; on battery it can disable the discrete GPU for maximum battery life. |

For a maximum-performance TensorCube benchmark session, the target profile is:

```text
Power state: AC connected
Armoury Crate operating mode: Turbo
GPU mode: Ultimate
Graphics route: discrete-GPU / MUX direct display path
```

This target profile is not a substitute for measurement-time capture. If the active configuration differs, report the actual configuration rather than silently normalizing it to the target profile.

The static platform description does not replace per-session environment capture. For every measured benchmark session, record at least:

```text
Windows version/build
NVIDIA driver version
AC/battery power state
active ROG/Armoury Crate operating mode
active GPU mode / MUX route
benchmark display resolution
pre-benchmark idle CPU utilization
pre-benchmark system-memory utilization
pre-benchmark discrete-GPU utilization and temperature
pre-benchmark disk and network activity summary
Tensor Core runtime state
executable SHA-256
```

Individual background processes do not need to be enumerated. A normal Windows idle state is acceptable, but intentional foreground workloads and obvious background transfers or scans should not overlap the measured solve interval.

Where GPU power or thermal behavior is material to a result, record the observed runtime power/temperature state rather than assuming that the nominal maximum graphics-power figure was sustained during the run.

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

1. Restart the executable and record OS, CPU, GPU, driver, power state, ROG operating mode, GPU mode/MUX route, and display resolution.
2. Confirm the machine is in a normal idle state and record the idle-state summary.
3. Perform an unmeasured warm-up sufficient to trigger shader compilation and GPU runtime initialization.
4. Generate legal states without supplying the solver a retained scramble history.
5. Measure solver time separately from playback/rendering time.
6. Run multiple independent states per order.
7. Record failures as failures.
8. Report at least mean, median, P95, and P99 wall-clock solve time.
9. Preserve exact-verification status for every measured run.

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
