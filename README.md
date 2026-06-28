# Fenrir Mosaic

Fenrir Mosaic is a C++17 scene and sprite parser with two libFuzzer entry points and ClusterFuzzLite wiring.

## Layout

- `src/mosaic/` - core parser, palette logic, render path, and generated catalog lookup
- `fuzz/` - fuzz targets, dictionaries, and seed corpus
- `.clusterfuzzlite/` - build script and fuzz target manifest

## Fuzz targets

- `parser_fuzzer` - parses and validates a complete scene
- `sprite_fuzzer` - parses a sprite sheet and exercises the contact-sheet path
