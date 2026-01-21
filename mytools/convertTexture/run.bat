mkdir tiles
mkdir tiles_4x

"c:\Program Files\ImageMagick-7.1.2-Q16-HDRI\magick.exe" BL32N0-0.DAT.bmp -crop 32x32 +repage tiles\tile_%%03d.png

"c:\Program Files\ImageMagick-7.1.2-Q16-HDRI\magick.exe" montage tiles_4x/tile_*.png -background none -tile 8x -geometry +0+0 BL128N0-0.DAT.bmp