#!/bin/bash -eu

mkdir -p "$OUT"

"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/scanner.cc" -o "$OUT/scanner.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/palette.cc" -o "$OUT/palette.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/rle.cc" -o "$OUT/rle.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/scene.cc" -o "$OUT/scene.o"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" -c "$SRC/src/mosaic/sprite_sheet.cc" -o "$OUT/sprite_sheet.o"

"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" "$SRC/fuzz/parser_fuzzer.cc" "$OUT/scanner.o" "$OUT/palette.o" "$OUT/rle.o" "$OUT/scene.o" "$OUT/sprite_sheet.o" $LIB_FUZZING_ENGINE -o "$OUT/parser_fuzzer"
"$CXX" $CXXFLAGS -std=c++17 -I"$SRC/src" "$SRC/fuzz/sprite_fuzzer.cc" "$OUT/scanner.o" "$OUT/palette.o" "$OUT/rle.o" "$OUT/scene.o" "$OUT/sprite_sheet.o" $LIB_FUZZING_ENGINE -o "$OUT/sprite_fuzzer"
