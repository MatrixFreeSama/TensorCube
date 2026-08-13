// TensorCube v32, portable Windows build for NxNxN cubes.
// Selectable orders: 2..49, plus stress extensions 100, 1000, 10000 and 100000.
// Freestanding Win32 + OpenGL, no external runtime dependencies.
// One matrix-free orbit-tensor provider and streaming factorization backend is shared by every selectable order.
// OpenGL 4.3 Compute performs matrix-free component factorization; a persistent CPU pool shares ready work.
// Random state is an implicit deterministic tensor descriptor; no generation history or task files are retained.
// Execution follows a work-conserving resource-return model: logical work is detached from executor identity,
// only the quotient reduction is a global dependency, and unrelated component chunks never wait for one another.

#define WIN32_LEAN_AND_MEAN

...