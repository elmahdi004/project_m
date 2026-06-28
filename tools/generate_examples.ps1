$root = 'C:\Users\user\Desktop\fenrir'
$sceneDir = Join-Path $root 'examples\scenes'
$sheetDir = Join-Path $root 'examples\sheets'
New-Item -ItemType Directory -Force -Path $sceneDir | Out-Null
New-Item -ItemType Directory -Force -Path $sheetDir | Out-Null

for ($i = 1; $i -le 120; $i++) {
  $width = 8 + ($i % 5)
  $height = 6 + ($i % 4)
  $paletteCount = 4 + ($i % 4)
  $layerCount = 2 + ($i % 3)
  $bgCount = 1 + ($i % 3)

  $scene = New-Object System.Text.StringBuilder
  [void]$scene.AppendLine(('scene {0} {1}' -f $width, $height))
  [void]$scene.AppendLine(('gradient {0}' -f $paletteCount))

  for ($c = 0; $c -lt $paletteCount; $c++) {
    $r = ($i * 37 + $c * 17) % 256
    $g = ($i * 53 + $c * 29) % 256
    $b = ($i * 71 + $c * 41) % 256
    [void]$scene.AppendLine(('#{0:x2}{1:x2}{2:x2}' -f $r, $g, $b))
  }

  $spans = @()
  for ($s = 0; $s -lt 12; $s++) {
    $count = 1 + (($i + $s) % 3)
    if (($s % 2) -eq 0) {
      $index = ($s + $i) % $paletteCount
      $spans += ('{0} ref {1}' -f $count, $index)
    } else {
      $r = ($i * 11 + $s * 33) % 256
      $g = ($i * 19 + $s * 27) % 256
      $b = ($i * 23 + $s * 31) % 256
      $spans += ('{0} lit #{1:x2}{2:x2}{3:x2}' -f $count, $r, $g, $b)
    }
  }

  [void]$scene.AppendLine(('rows {0} {1}' -f $height, $spans.Count))
  foreach ($span in $spans) {
    [void]$scene.AppendLine($span)
  }

  [void]$scene.AppendLine($layerCount)
  for ($l = 0; $l -lt $layerCount; $l++) {
    [void]$scene.AppendLine(('layer_{0:D2}_{1}' -f $i, $l))
    if (($l % 2) -eq 0) {
      [void]$scene.AppendLine('1')
    } else {
      [void]$scene.AppendLine('0')
    }
  }

  [void]$scene.AppendLine($bgCount)
  for ($b = 0; $b -lt $bgCount; $b++) {
    $r = ($i * 13 + $b * 97) % 256
    $g = ($i * 17 + $b * 89) % 256
    $bb = ($i * 19 + $b * 83) % 256
    [void]$scene.AppendLine(('#{0:x2}{1:x2}{2:x2}' -f $r, $g, $bb))
  }

  Set-Content -Path (Join-Path $sceneDir ('scene_{0:D2}.mosaic' -f $i)) -Value $scene.ToString() -NoNewline
}

for ($i = 1; $i -le 60; $i++) {
  $sheet = New-Object System.Text.StringBuilder
  [void]$sheet.AppendLine(('sheet atlas_{0:D2}' -f $i))
  [void]$sheet.AppendLine((8 + ($i % 4)))
  [void]$sheet.AppendLine((8 + ($i % 3)))
  [void]$sheet.AppendLine((4 + ($i % 5)))
  for ($t = 0; $t -lt (4 + ($i % 5)); $t++) {
    [void]$sheet.AppendLine(('tile_{0:D2}_{1}' -f $i, $t))
  }
  Set-Content -Path (Join-Path $sheetDir ('sheet_{0:D2}.atlas' -f $i)) -Value $sheet.ToString() -NoNewline
}
