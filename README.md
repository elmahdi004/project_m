# Fenrir Mosaic

Fenrir Mosaic is a small C++17 scene toolchain for working with layered artwork. It parses a compact text scene format, resolves palette data, expands run-length encoded spans, composites the layers into a raster image, and can write that result as a PPM file or a compact ASCII preview. The repository also includes a sprite-sheet inspector for checking atlas metadata and a command-line interface for rendering or reviewing inputs from the shell.

The project is organized as a real application rather than a harness wrapper. The core library lives under `src/mosaic/`, with reusable pieces for parsing, palette handling, image manipulation, montage building, report generation, and PPM output. The `examples/` directory contains a larger set of scene and sprite-sheet documents that exercise the CLI in realistic ways, while `fuzz/` keeps the libFuzzer entry points for parser and sprite-sheet validation.

## What it does

- Parse scene documents that describe dimensions, palettes, spans, layers, and background colors.
- Render a scene into an image buffer and export it as binary or ASCII PPM.
- Inspect scene statistics such as layer count, color diversity, and a text preview.
- Inspect sprite-sheet documents and summarize their tile metadata.
- Build a montage from a directory of scenes for quick visual review.

## Layout

- `src/mosaic/` - reusable library code for parsing, rendering, images, reports, and output formats
- `src/main.cc` - CLI entry point that wires the library into the `mosaicc` tool
- `examples/` - sample scenes and sprite sheets you can render or inspect immediately
- `fuzz/` - libFuzzer entry points, dictionaries, and seed corpus for parser validation
- `.clusterfuzzlite/` - build script and fuzz-target manifest

## Commands

- `mosaicc inspect <scene-file>` prints a short report and ASCII preview.
- `mosaicc render <scene-file> [output.ppm] [--ascii] [--preset name]` writes a rendered scene.
- `mosaicc spritesheet <sheet-file>` prints the atlas summary.
- `mosaicc montage <scene-directory> [output.ppm] [--ascii] [--preset name]` builds a contact sheet from scene files.

## Fuzz targets

- `parser_fuzzer` - exercises the scene parser and renderer path.
- `sprite_fuzzer` - exercises the sprite-sheet loader and contact-sheet path.

## Example data

The repository ships with multiple ready-to-use inputs under `examples/scenes/` and `examples/sheets/`. They are useful for manual checks, smoke-testing the CLI, and giving the fuzz targets structured starting points.
