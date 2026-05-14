#!/usr/bin/env python3
"""
make-icon.py - Genere l'icone PrintBoy (printer-themed) sans dependance.
Style : carre orange Prusa avec une silhouette d'imprimante stylisee.
"""
import os
import struct
import zlib


def png_chunk(tag, data):
    chunk = tag + data
    crc = zlib.crc32(chunk) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + chunk + struct.pack(">I", crc)


def encode_png(width, height, pixels):
    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    stride = width * 4
    raw = b"".join(b"\x00" + pixels[i * stride:(i + 1) * stride] for i in range(height))
    return sig + png_chunk(b"IHDR", ihdr) + png_chunk(b"IDAT", zlib.compress(raw, 9)) + png_chunk(b"IEND", b"")


def draw(size):
    pix = bytearray(size * size * 4)

    def put(x, y, rgba):
        if 0 <= x < size and 0 <= y < size:
            o = (y * size + x) * 4
            pix[o:o + 4] = bytes(rgba)

    def rect(x, y, w, h, rgba):
        for yy in range(max(0, y), min(size, y + h)):
            for xx in range(max(0, x), min(size, x + w)):
                put(xx, yy, rgba)

    bg     = (255, 132, 20, 255)    # Prusa orange
    dark   = (180, 80,  10, 255)
    light  = (255, 200, 130, 255)
    white  = (255, 255, 255, 255)
    panel  = (40, 40, 50, 255)

    # Disque de fond plein (carre coins arrondis simules)
    s = size
    for y in range(s):
        for x in range(s):
            # carre avec coins legerement arrondis
            dx = min(x, s - 1 - x)
            dy = min(y, s - 1 - y)
            corner = min(dx, dy)
            if corner >= s * 0.04:
                put(x, y, bg)

    # Cadre interne
    rect(int(s * 0.10), int(s * 0.10), int(s * 0.80), int(s * 0.80), bg)

    # Silhouette imprimante stylisee : base + portique + tete
    # Plateau
    bx = int(s * 0.18); by = int(s * 0.62); bw = int(s * 0.64); bh = int(s * 0.06)
    rect(bx, by, bw, bh, dark)
    rect(bx, by + bh, bw, max(2, int(s * 0.02)), panel)

    # Colonnes verticales
    cw = max(3, int(s * 0.05))
    rect(bx, int(s * 0.20), cw, by - int(s * 0.20), dark)
    rect(bx + bw - cw, int(s * 0.20), cw, by - int(s * 0.20), dark)

    # Traverse haute
    rect(bx, int(s * 0.20), bw, max(3, int(s * 0.04)), dark)

    # Tete d'impression (cube avec filament)
    hx = int(s * 0.42); hy = int(s * 0.28); hw = int(s * 0.16); hh = int(s * 0.14)
    rect(hx, hy, hw, hh, panel)
    # buse pointue
    nz_y = hy + hh
    for k in range(int(s * 0.04)):
        wd = max(1, int(s * 0.04) - k)
        rect(hx + hw // 2 - wd // 2, nz_y + k, wd, 1, panel)

    # Filament rouge qui sort en dessous
    fx = hx + hw // 2
    rect(fx - 1, nz_y + int(s * 0.04), 2, int(s * 0.10), (220, 30, 30, 255))

    # Petit indicateur LED blanc
    rect(hx + int(s * 0.02), hy + int(s * 0.02),
         max(2, int(s * 0.03)), max(2, int(s * 0.03)), light)

    # Lueur blanche sur la base (effet print)
    rect(bx + int(s * 0.05), by - int(s * 0.02),
         bw - int(s * 0.10), max(1, int(s * 0.02)), white)

    return bytes(pix)


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    print("Generation icones PrintBoy...")
    for size, dest in [
        (64,  os.path.join(root, "Icons", "Default", "app", "printboy.png")),
        (64,  os.path.join(root, "App", "PrintBoy", "res", "printboy_icon.png")),
        (256, os.path.join(root, "preview", "printboy_icon_256.png")),
    ]:
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        with open(dest, "wb") as f:
            f.write(encode_png(size, size, draw(size)))
        print(f"  -> {dest} ({size}x{size})")
    print("OK")


if __name__ == "__main__":
    main()
