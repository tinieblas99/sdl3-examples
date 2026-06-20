#!/usr/bin/env python3
from pathlib import Path
import math

WIDTH = 192
HEIGHT = 128
WATER_FIRST = 32
WATER_COLORS = 48


def main():
    out_dir = Path(__file__).resolve().parent / "assets"
    out_dir.mkdir(parents=True, exist_ok=True)
    pixels = bytearray()

    for y in range(HEIGHT):
        for x in range(WIDTH):
            wave_a = math.sin((x * 0.16) + (y * 0.09))
            wave_b = math.sin((x * 0.05) - (y * 0.19))
            ripple = math.sin(math.hypot(x - 96, y - 60) * 0.22)
            value = wave_a * 16.0 + wave_b * 10.0 + ripple * 8.0 + y * 0.08
            band = int(value) % WATER_COLORS
            pixels.append(WATER_FIRST + band)

    output = out_dir / "water_indices.bin"
    output.write_bytes(pixels)
    print(f"wrote {output} ({WIDTH}x{HEIGHT}, 8-bit indices)")


if __name__ == "__main__":
    main()
