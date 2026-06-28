#!/bin/bash -eu

mkdir -p "$OUT"

"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/scanner.cc" -o "$OUT/scanner.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/palette.cc" -o "$OUT/palette.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/rle.cc" -o "$OUT/rle.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/scene.cc" -o "$OUT/scene.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/scene_io.cc" -o "$OUT/scene_io.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/sprite_sheet.cc" -o "$OUT/sprite_sheet.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/image.cc" -o "$OUT/image.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/ppm.cc" -o "$OUT/ppm.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/report.cc" -o "$OUT/report.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/cli.cc" -o "$OUT/cli.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/image_ops.cc" -o "$OUT/image_ops.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/montage.cc" -o "$OUT/montage.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/style_presets.cc" -o "$OUT/style_presets.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/main.cc" -o "$OUT/main.o"

"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" "$SRC/fuzz/parser_fuzzer.cc" "$OUT/scanner.o" "$OUT/palette.o" "$OUT/rle.o" "$OUT/scene.o" "$OUT/sprite_sheet.o" $LIB_FUZZING_ENGINE -o "$OUT/parser_fuzzer"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" "$SRC/fuzz/sprite_fuzzer.cc" "$OUT/scanner.o" "$OUT/palette.o" "$OUT/rle.o" "$OUT/scene.o" "$OUT/sprite_sheet.o" $LIB_FUZZING_ENGINE -o "$OUT/sprite_fuzzer"

"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" "$OUT/main.o" "$OUT/cli.o" "$OUT/image.o" "$OUT/image_ops.o" "$OUT/montage.o" "$OUT/scene_io.o" "$OUT/style_presets.o" "$OUT/ppm.o" "$OUT/report.o" "$OUT/scanner.o" "$OUT/palette.o" "$OUT/rle.o" "$OUT/scene.o" "$OUT/sprite_sheet.o" -o "$OUT/mosaicc"
