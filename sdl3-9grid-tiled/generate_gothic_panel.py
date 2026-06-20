#!/usr/bin/env python3
from pathlib import Path
import math
import struct
import zlib

SIZE = 96
CORNER = 24

DARK = (18, 12, 26, 255)
CENTER_A = (32, 18, 38, 255)
CENTER_B = (41, 24, 48, 255)
STONE = (54, 49, 64, 255)
STONE_DARK = (31, 27, 38, 255)
GOLD = (186, 139, 54, 255)
GOLD_BRIGHT = (240, 205, 106, 255)
RED = (92, 14, 29, 255)
RED_BRIGHT = (153, 31, 45, 255)
BLUE_GLOW = (73, 102, 146, 255)
SHADOW = (8, 6, 12, 255)


def blend(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(4))


def set_px(pixels, x, y, color):
    if 0 <= x < SIZE and 0 <= y < SIZE:
        pixels[y][x] = color


def fill_rect(pixels, x, y, w, h, color):
    for yy in range(y, y + h):
        for xx in range(x, x + w):
            set_px(pixels, xx, yy, color)


def draw_line(pixels, x0, y0, x1, y1, color):
    dx = abs(x1 - x0)
    sx = 1 if x0 < x1 else -1
    dy = -abs(y1 - y0)
    sy = 1 if y0 < y1 else -1
    err = dx + dy
    while True:
        set_px(pixels, x0, y0, color)
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x0 += sx
        if e2 <= dx:
            err += dx
            y0 += sy


def draw_circle(pixels, cx, cy, radius, color):
    for y in range(cy - radius, cy + radius + 1):
        for x in range(cx - radius, cx + radius + 1):
            if (x - cx) * (x - cx) + (y - cy) * (y - cy) <= radius * radius:
                set_px(pixels, x, y, color)


def decorate_corner(pixels, ox, oy, flip_x, flip_y):
    sx = -1 if flip_x else 1
    sy = -1 if flip_y else 1
    ax = ox + (23 if flip_x else 0)
    ay = oy + (23 if flip_y else 0)

    for i in range(24):
        t = i / 23
        color = blend(STONE_DARK, STONE, t)
        for j in range(24 - i):
            set_px(pixels, ax + sx * i, ay + sy * j, color)
            set_px(pixels, ax + sx * j, ay + sy * i, color)

    draw_line(pixels, ax, ay + sy * 22, ax + sx * 22, ay, SHADOW)
    draw_line(pixels, ax + sx * 3, ay + sy * 20, ax + sx * 20, ay + sy * 3, GOLD)
    draw_line(pixels, ax + sx * 5, ay + sy * 18, ax + sx * 18, ay + sy * 5, GOLD_BRIGHT)

    cx = ox + (9 if not flip_x else 14)
    cy = oy + (9 if not flip_y else 14)
    draw_circle(pixels, cx, cy, 6, RED)
    draw_circle(pixels, cx, cy, 4, RED_BRIGHT)
    draw_circle(pixels, cx, cy, 2, GOLD_BRIGHT)


def main():
    pixels = [[DARK for _ in range(SIZE)] for _ in range(SIZE)]

    for y in range(CORNER, SIZE - CORNER):
        for x in range(CORNER, SIZE - CORNER):
            checker = ((x // 8) + (y // 8)) % 2
            wave = 0.5 + 0.5 * math.sin(x * 0.55 + y * 0.35)
            base = CENTER_A if checker else CENTER_B
            color = blend(base, BLUE_GLOW, wave * 0.16)
            set_px(pixels, x, y, color)

    for x in range(CORNER, SIZE - CORNER):
        motif = (x - CORNER) % 12
        fill_rect(pixels, x, 0, 1, CORNER, STONE_DARK if motif < 2 else STONE)
        fill_rect(pixels, x, SIZE - CORNER, 1, CORNER, STONE_DARK if motif < 2 else STONE)
        if motif in (4, 5, 6):
            fill_rect(pixels, x, 4, 1, 16, GOLD)
            fill_rect(pixels, x, SIZE - 20, 1, 16, GOLD)
        if motif == 6:
            fill_rect(pixels, x, 7, 1, 10, GOLD_BRIGHT)
            fill_rect(pixels, x, SIZE - 17, 1, 10, GOLD_BRIGHT)

    for y in range(CORNER, SIZE - CORNER):
        motif = (y - CORNER) % 12
        fill_rect(pixels, 0, y, CORNER, 1, STONE_DARK if motif < 2 else STONE)
        fill_rect(pixels, SIZE - CORNER, y, CORNER, 1, STONE_DARK if motif < 2 else STONE)
        if motif in (4, 5, 6):
            fill_rect(pixels, 4, y, 16, 1, GOLD)
            fill_rect(pixels, SIZE - 20, y, 16, 1, GOLD)
        if motif == 6:
            fill_rect(pixels, 7, y, 10, 1, GOLD_BRIGHT)
            fill_rect(pixels, SIZE - 17, y, 10, 1, GOLD_BRIGHT)

    fill_rect(pixels, CORNER, CORNER - 2, SIZE - CORNER * 2, 2, SHADOW)
    fill_rect(pixels, CORNER, SIZE - CORNER, SIZE - CORNER * 2, 2, GOLD_BRIGHT)
    fill_rect(pixels, CORNER - 2, CORNER, 2, SIZE - CORNER * 2, SHADOW)
    fill_rect(pixels, SIZE - CORNER, CORNER, 2, SIZE - CORNER * 2, GOLD_BRIGHT)

    decorate_corner(pixels, 0, 0, False, False)
    decorate_corner(pixels, SIZE - CORNER, 0, True, False)
    decorate_corner(pixels, 0, SIZE - CORNER, False, True)
    decorate_corner(pixels, SIZE - CORNER, SIZE - CORNER, True, True)

    raw = bytearray()
    for row in pixels:
        raw.append(0)
        for r, g, b, a in row:
            raw.extend((r, g, b, a))

    def chunk(kind, data):
        body = kind + data
        return struct.pack('>I', len(data)) + body + struct.pack('>I', zlib.crc32(body) & 0xffffffff)

    png = bytearray([137, 80, 78, 71, 13, 10, 26, 10])
    png += chunk(b'IHDR', struct.pack('>IIBBBBB', SIZE, SIZE, 8, 6, 0, 0, 0))
    png += chunk(b'IDAT', zlib.compress(bytes(raw), 9))
    png += chunk(b'IEND', b'')

    out_dir = Path(__file__).resolve().parent / 'assets'
    out_dir.mkdir(parents=True, exist_ok=True)
    output = out_dir / 'gothic_panel_9grid.png'
    output.write_bytes(png)
    print(f'wrote {output} ({SIZE}x{SIZE} RGBA PNG, {CORNER}px corners)')


if __name__ == '__main__':
    main()
