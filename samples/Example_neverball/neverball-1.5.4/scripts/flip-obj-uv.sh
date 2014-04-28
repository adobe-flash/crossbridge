#!/bin/sh

# Use this script to invert the vertical texture coordinates in a
# Wavefront OBJ file.

for obj; do
    LC_ALL=C awk '
/^vt / {
    $3 = sprintf("%e", 1.0 - strtonum($3))
}
{ print }
' "$obj" > "$obj".tmp
    mv -f "$obj".tmp "$obj"
done

