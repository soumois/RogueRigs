param([string]$In, [string]$Out)
Add-Type -AssemblyName System.Drawing
$img = [System.Drawing.Image]::FromFile($In)
$bmp = New-Object System.Drawing.Bitmap($img.Width, $img.Height)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = 'AntiAlias'
$g.DrawImage($img, 0, 0, $img.Width, $img.Height)

# ground frame: origin at the miner's feet, aim angle read from the bullet tracers, vertical foreshortening k
$ox = 950.0; $oy = 588.0
$theta = 46.0 * [Math]::PI / 180.0
$k = 0.60
$cos = [Math]::Cos($theta); $sin = [Math]::Sin($theta)

function Pt($u, $v) {
    $gx = $u * $cos - $v * $sin
    $gy = $u * $sin + $v * $cos
    return New-Object System.Drawing.PointF(($ox + $gx), ($oy - $gy * $k))
}

# chevron outline in (forward u, lateral v)
function Chevron($t, $L, $h, $th) {
    return @(
        (Pt $t 0), (Pt ($t - $L) $h), (Pt ($t - $L - $th) $h),
        (Pt ($t - $th) 0), (Pt ($t - $L - $th) (-$h)), (Pt ($t - $L) (-$h))
    )
}

$count = 5
for ($i = 0; $i -lt $count; $i++) {
    $t = 120 + $i * 74
    $a = [int](255 * [Math]::Pow(0.78, $i))
    $pts = Chevron $t 46 27 17
    $glow = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb([int]($a * 0.35), 255, 226, 140), 9)
    $glow.LineJoin = 'Round'
    $g.DrawPolygon($glow, $pts)
    $fill = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb($a, 255, 236, 170))
    $g.FillPolygon($fill, $pts)
}
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose(); $img.Dispose()
