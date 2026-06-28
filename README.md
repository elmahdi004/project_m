# Fenrir Mosaic

Fenrir Mosaic is a small C++17 content pipeline that parses a scene description, loads palette data, expands run-length encoded pixel spans, and composites sprite layers into an image-like buffer. The repository is structured as a real fuzzing target: the parser and renderer are reachable through two separate libFuzzer entry points, and the build is wired for ClusterFuzzLite so the project can be compiled and fuzzed without interactive setup.

The code is intentionally split across several modules so fuzzing can exercise more than one path. `parser_fuzzer` drives the scene parser, palette handling, span decoding, and compositing flow. `sprite_fuzzer` targets the sprite-sheet loader and contact-sheet formatting path. A large generated catalog module is included to keep the project substantial and to provide a realistic, data-heavy lookup path instead of a toy stub.

## Layout

- `src/mosaic/` - core parser, palette logic, render path, and generated catalog lookup
- `fuzz/` - fuzz targets, dictionaries, and seed corpus
- `.clusterfuzzlite/` - build script and fuzz target manifest

## Fuzz targets

- `parser_fuzzer` - parses and validates a complete scene
- `sprite_fuzzer` - parses a sprite sheet and exercises the contact-sheet path

## Purpose

This repository is meant to be a standalone, private fuzzing submission. The goal is to provide a non-trivial codebase that can surface real crashes under sanitizers and then be patched without disabling the harness or weakening coverage. The seed corpus is there to help the fuzzer reach structured input paths quickly, while the generated catalog and multiple entry points keep the repository large enough and varied enough to behave like a real target rather than a minimal example.

## Build shape

The ClusterFuzzLite build script compiles every harness into `$OUT` from the repository root. That means the sources, fuzz targets, and generated lookup data are all self-contained in this tree, and the build does not depend on network access, private credentials, or local machine-specific paths.
