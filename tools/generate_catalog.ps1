$path = 'C:\Users\user\Desktop\fenrir\src\mosaic\generated_catalog_big.cc'
$count = 10000
$sb = New-Object System.Text.StringBuilder

[void]$sb.AppendLine('#include "mosaic/generated_catalog.h"')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('#include <array>')
[void]$sb.AppendLine('#include <cstddef>')
[void]$sb.AppendLine('#include <string_view>')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('namespace mosaic {')
[void]$sb.AppendLine('namespace {')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('constexpr char kCatalogBlob[] = R"CATALOG(')

for ($i = 0; $i -lt $count; ++$i) {
  $name = 'catalog_' + $i.ToString('D5')
  $r1 = ($i * 17) % 256
  $g1 = ($i * 31) % 256
  $b1 = ($i * 43) % 256
  $r2 = ($i * 53 + 11) % 256
  $g2 = ($i * 67 + 17) % 256
  $b2 = ($i * 79 + 23) % 256
  $r3 = ($i * 97 + 29) % 256
  $g3 = ($i * 101 + 31) % 256
  $b3 = ($i * 113 + 37) % 256
  $r4 = ($i * 127 + 41) % 256
  $g4 = ($i * 131 + 43) % 256
  $b4 = ($i * 137 + 47) % 256
  $r5 = ($i * 139 + 53) % 256
  $g5 = ($i * 149 + 59) % 256
  $b5 = ($i * 157 + 61) % 256
  $r6 = ($i * 163 + 67) % 256
  $g6 = ($i * 173 + 71) % 256
  $b6 = ($i * 179 + 73) % 256
  $r7 = ($i * 181 + 79) % 256
  $g7 = ($i * 191 + 83) % 256
  $b7 = ($i * 193 + 89) % 256
  $r8 = ($i * 197 + 97) % 256
  $g8 = ($i * 199 + 101) % 256
  $b8 = ($i * 211 + 103) % 256
  $pad = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
  $line = $name + '|' + $r1 + ',' + $g1 + ',' + $b1 + ',255|' + $r2 + ',' + $g2 + ',' + $b2 + ',255|' + $r3 + ',' + $g3 + ',' + $b3 + ',255|' + $r4 + ',' + $g4 + ',' + $b4 + ',255|' + $r5 + ',' + $g5 + ',' + $b5 + ',255|' + $r6 + ',' + $g6 + ',' + $b6 + ',255|' + $r7 + ',' + $g7 + ',' + $b7 + ',255|' + $r8 + ',' + $g8 + ',' + $b8 + ',255|' + $pad
  [void]$sb.AppendLine($line)
}

[void]$sb.AppendLine(')CATALOG";')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('bool ParseField(std::string_view text, std::size_t* index, uint8_t* value) {')
[void]$sb.AppendLine('  std::size_t cursor = *index;')
[void]$sb.AppendLine('  if (cursor >= text.size()) {')
[void]$sb.AppendLine('    return false;')
[void]$sb.AppendLine('  }')
[void]$sb.AppendLine('  unsigned int result = 0;')
[void]$sb.AppendLine('  bool saw_digit = false;')
[void]$sb.AppendLine('  while (cursor < text.size()) {')
[void]$sb.AppendLine('    char ch = text[cursor];')
[void]$sb.AppendLine("    if (ch < '0' || ch > '9') {")
[void]$sb.AppendLine('      break;')
[void]$sb.AppendLine('    }')
[void]$sb.AppendLine('    saw_digit = true;')
[void]$sb.AppendLine("    result = result * 10 + static_cast<unsigned int>(ch - '0');")
[void]$sb.AppendLine('    if (result > 255U) {')
[void]$sb.AppendLine('      return false;')
[void]$sb.AppendLine('    }')
[void]$sb.AppendLine('    ++cursor;')
[void]$sb.AppendLine('  }')
[void]$sb.AppendLine('  if (!saw_digit) {')
[void]$sb.AppendLine('    return false;')
[void]$sb.AppendLine('  }')
[void]$sb.AppendLine('  *value = static_cast<uint8_t>(result);')
[void]$sb.AppendLine('  *index = cursor;')
[void]$sb.AppendLine('  return true;')
[void]$sb.AppendLine('}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('bool ParseColorField(std::string_view text, std::size_t* index, Color* color) {')
[void]$sb.AppendLine('  if (!ParseField(text, index, &color->r)) return false;')
[void]$sb.AppendLine("  if (*index >= text.size() || text[*index] != ',') return false;")
[void]$sb.AppendLine('  ++(*index);')
[void]$sb.AppendLine('  if (!ParseField(text, index, &color->g)) return false;')
[void]$sb.AppendLine("  if (*index >= text.size() || text[*index] != ',') return false;")
[void]$sb.AppendLine('  ++(*index);')
[void]$sb.AppendLine('  if (!ParseField(text, index, &color->b)) return false;')
[void]$sb.AppendLine("  if (*index >= text.size() || text[*index] != ',') return false;")
[void]$sb.AppendLine('  ++(*index);')
[void]$sb.AppendLine('  if (!ParseField(text, index, &color->a)) return false;')
[void]$sb.AppendLine('  return true;')
[void]$sb.AppendLine('}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('}  // namespace')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('bool LookupPresetPalette(const std::string& name, std::vector<Color>* colors) {')
[void]$sb.AppendLine('  std::string_view blob(kCatalogBlob);')
[void]$sb.AppendLine('  std::size_t line_start = 0;')
[void]$sb.AppendLine('  while (line_start < blob.size()) {')
[void]$sb.AppendLine("    std::size_t line_end = blob.find('\n', line_start);")
[void]$sb.AppendLine('    if (line_end == std::string_view::npos) {')
[void]$sb.AppendLine('      line_end = blob.size();')
[void]$sb.AppendLine('    }')
[void]$sb.AppendLine('    std::string_view line = blob.substr(line_start, line_end - line_start);')
[void]$sb.AppendLine("    if (line.size() > name.size() + 1 && line.compare(0, name.size(), name) == 0 && line[name.size()] == '|') {")
[void]$sb.AppendLine('      std::size_t cursor = name.size() + 1;')
[void]$sb.AppendLine('      std::array<Color, 8> parsed_colors{};')
[void]$sb.AppendLine('      for (Color& color : parsed_colors) {')
[void]$sb.AppendLine('        if (!ParseColorField(line, &cursor, &color)) {')
[void]$sb.AppendLine('          return false;')
[void]$sb.AppendLine('        }')
[void]$sb.AppendLine("        if (cursor < line.size() && line[cursor] == '|') {")
[void]$sb.AppendLine('          ++cursor;')
[void]$sb.AppendLine('        }')
[void]$sb.AppendLine('      }')
[void]$sb.AppendLine('      colors->assign(parsed_colors.begin(), parsed_colors.end());')
[void]$sb.AppendLine('      return true;')
[void]$sb.AppendLine('    }')
[void]$sb.AppendLine('    line_start = line_end + 1;')
[void]$sb.AppendLine('  }')
[void]$sb.AppendLine('  return false;')
[void]$sb.AppendLine('}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('std::size_t PresetPaletteCount() {')
[void]$sb.AppendLine('  return 10000;')
[void]$sb.AppendLine('}')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('}  // namespace mosaic')

[System.IO.File]::WriteAllText($path, $sb.ToString())