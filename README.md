# cheap

Simple single-thread, general-purpose arena allocator implemented in C.

[![CI](https://github.com/migopp/cheap/actions/workflows/ci.yml/badge.svg)](https://github.com/migopp/cheap/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Usage

As of now, you will have to build for your platform manually. Simply link with the object file(s) of your chosen allocator(s).

Peek at `src/arena/arena.h` for the general interface. In general, note the following:

```c
// Initialize an arena with `AllocatorType`
arena_allocator arena_init(AllocatorType);

// Allocate memory in the given arena with appropriate size
void *arena_malloc(arena_allocator *, size_t);

// Free memory in the given arena
void arena_free(arena_allocator *, void *);

// Deinitialize the arena
void arena_deinit(arena_allocator *);
```

## Allocators

`cheap` exposes an arena in `arena/arena.h`; but the underlying allocator used is user-configurable via `AllocatorType`.

Provided allocators include:

- Buddy allocator: `CHEAP_BUDDY`
- Bump allocator: `CHEAP_BUMP`
- Free list allocator: `CHEAP_FL`
- Pool allocator: `CHEAP_POOL`
- Stack allocator: `CHEAP_STACK`

## Building

Clone this repository,

```
git clone https://github.com/migopp/cheap.git
```

Then use `make` or manually compile your desired allocators:

```
make release
```

## Testing

Testing files and infrastructure are available in `test/`.

## Benchmarking

Limited benchmarking infrastructure is available at `benchmark/`.
