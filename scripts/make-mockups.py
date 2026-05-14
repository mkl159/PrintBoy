#!/usr/bin/env python3
"""
PrintBoy - mockups d'ecran 640x480, stdlib only.
"""
import os
import struct
import zlib
import math

W, H = 640, 480


def png_chunk(tag, data):
    chunk = tag + data
    crc = zlib.crc32(chunk) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + chunk + struct.pack(">I", crc)


def encode_png(w, h, pixels):
    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)
    stride = w * 4
    raw = b"".join(b"\x00" + pixels[i * stride:(i + 1) * stride] for i in range(h))
    return sig + png_chunk(b"IHDR", ihdr) + png_chunk(b"IDAT", zlib.compress(raw, 9)) + png_chunk(b"IEND", b"")


# Palette PrintBoy
BG     = (18, 20, 28, 255)
FG     = (240, 240, 245, 255)
DIM    = (140, 140, 160, 255)
ACCENT = (255, 132, 20, 255)
PANEL  = (32, 36, 48, 255)
OK     = (60, 200, 100, 255)
WARN   = (245, 200, 60, 255)
ERR    = (230, 80, 80, 255)


class C:
    FONT = {
        " ": ["     "] * 7,
        "A": [" XXX ", "X   X", "X   X", "XXXXX", "X   X", "X   X", "X   X"],
        "B": ["XXXX ", "X   X", "X   X", "XXXX ", "X   X", "X   X", "XXXX "],
        "C": [" XXXX", "X    ", "X    ", "X    ", "X    ", "X    ", " XXXX"],
        "D": ["XXXX ", "X   X", "X   X", "X   X", "X   X", "X   X", "XXXX "],
        "E": ["XXXXX", "X    ", "X    ", "XXX  ", "X    ", "X    ", "XXXXX"],
        "F": ["XXXXX", "X    ", "X    ", "XXX  ", "X    ", "X    ", "X    "],
        "G": [" XXXX", "X    ", "X    ", "X  XX", "X   X", "X   X", " XXXX"],
        "H": ["X   X", "X   X", "X   X", "XXXXX", "X   X", "X   X", "X   X"],
        "I": ["XXXXX", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "XXXXX"],
        "J": ["XXXXX", "    X", "    X", "    X", "    X", "X   X", " XXX "],
        "K": ["X   X", "X  X ", "X X  ", "XX   ", "X X  ", "X  X ", "X   X"],
        "L": ["X    ", "X    ", "X    ", "X    ", "X    ", "X    ", "XXXXX"],
        "M": ["X   X", "XX XX", "X X X", "X X X", "X   X", "X   X", "X   X"],
        "N": ["X   X", "XX  X", "X X X", "X X X", "X  XX", "X   X", "X   X"],
        "O": [" XXX ", "X   X", "X   X", "X   X", "X   X", "X   X", " XXX "],
        "P": ["XXXX ", "X   X", "X   X", "XXXX ", "X    ", "X    ", "X    "],
        "Q": [" XXX ", "X   X", "X   X", "X   X", "X X X", "X  X ", " XX X"],
        "R": ["XXXX ", "X   X", "X   X", "XXXX ", "X X  ", "X  X ", "X   X"],
        "S": [" XXXX", "X    ", "X    ", " XXX ", "    X", "    X", "XXXX "],
        "T": ["XXXXX", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "  X  "],
        "U": ["X   X", "X   X", "X   X", "X   X", "X   X", "X   X", " XXX "],
        "V": ["X   X", "X   X", "X   X", "X   X", "X   X", " X X ", "  X  "],
        "W": ["X   X", "X   X", "X   X", "X X X", "X X X", "XX XX", "X   X"],
        "X": ["X   X", "X   X", " X X ", "  X  ", " X X ", "X   X", "X   X"],
        "Y": ["X   X", "X   X", " X X ", "  X  ", "  X  ", "  X  ", "  X  "],
        "Z": ["XXXXX", "    X", "   X ", "  X  ", " X   ", "X    ", "XXXXX"],
        "0": [" XXX ", "X   X", "X  XX", "X X X", "XX  X", "X   X", " XXX "],
        "1": ["  X  ", " XX  ", "X X  ", "  X  ", "  X  ", "  X  ", "XXXXX"],
        "2": [" XXX ", "X   X", "    X", "   X ", "  X  ", " X   ", "XXXXX"],
        "3": [" XXX ", "X   X", "    X", "  XX ", "    X", "X   X", " XXX "],
        "4": ["   X ", "  XX ", " X X ", "X  X ", "XXXXX", "   X ", "   X "],
        "5": ["XXXXX", "X    ", "X    ", "XXXX ", "    X", "X   X", " XXX "],
        "6": [" XXX ", "X    ", "X    ", "XXXX ", "X   X", "X   X", " XXX "],
        "7": ["XXXXX", "    X", "   X ", "  X  ", " X   ", " X   ", " X   "],
        "8": [" XXX ", "X   X", "X   X", " XXX ", "X   X", "X   X", " XXX "],
        "9": [" XXX ", "X   X", "X   X", " XXXX", "    X", "    X", " XXX "],
        ".": ["     ", "     ", "     ", "     ", "     ", "     ", "  X  "],
        ",": ["     ", "     ", "     ", "     ", "     ", "  X  ", " X   "],
        ":": ["     ", "  X  ", "     ", "     ", "     ", "  X  ", "     "],
        "!": ["  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "     ", "  X  "],
        "/": ["    X", "    X", "   X ", "  X  ", " X   ", "X    ", "X    "],
        "-": ["     ", "     ", "     ", "XXXXX", "     ", "     ", "     "],
        "+": ["     ", "  X  ", "  X  ", "XXXXX", "  X  ", "  X  ", "     "],
        "<": ["    X", "   X ", "  X  ", " X   ", "  X  ", "   X ", "    X"],
        ">": ["X    ", " X   ", "  X  ", "   X ", "  X  ", " X   ", "X    "],
        "[": [" XXX ", " X   ", " X   ", " X   ", " X   ", " X   ", " XXX "],
        "]": [" XXX ", "   X ", "   X ", "   X ", "   X ", "   X ", " XXX "],
        "(": ["   X ", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "   X "],
        ")": [" X   ", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", " X   "],
        "%": ["XX  X", "XX X ", "  X  ", " X   ", "X  XX", "  X  ", "X   X"],  # approx
        "_": ["     ", "     ", "     ", "     ", "     ", "     ", "XXXXX"],
        "@": [" XXX ", "X   X", "X  XX", "X X X", "X XX ", "X    ", " XXX "],
        "#": [" X X ", " X X ", "XXXXX", " X X ", "XXXXX", " X X ", " X X "],
        "*": ["     ", "X X X", " XXX ", "XXXXX", " XXX ", "X X X", "     "],
        "=": ["     ", "     ", "XXXXX", "     ", "XXXXX", "     ", "     "],
        "?": [" XXX ", "X   X", "    X", "   X ", "  X  ", "     ", "  X  "],
        "'": ["  X  ", "  X  ", "     ", "     ", "     ", "     ", "     "],
        '"': [" X X ", " X X ", "     ", "     ", "     ", "     ", "     "],
    }

    def __init__(self, w, h, bg):
        self.w, self.h = w, h
        self.pix = bytearray(w * h * 4)
        for i in range(0, len(self.pix), 4):
            self.pix[i:i + 4] = bytes(bg)

    def put(self, x, y, c):
        if 0 <= x < self.w and 0 <= y < self.h:
            o = (y * self.w + x) * 4
            self.pix[o:o + 4] = bytes(c)

    def rect(self, x, y, w, h, c):
        for yy in range(max(0, y), min(self.h, y + h)):
            for xx in range(max(0, x), min(self.w, x + w)):
                self.put(xx, yy, c)

    def rect_out(self, x, y, w, h, c, t=1):
        self.rect(x, y, w, t, c)
        self.rect(x, y + h - t, w, t, c)
        self.rect(x, y, t, h, c)
        self.rect(x + w - t, y, t, h, c)

    def text(self, x, y, s, c=FG, scale=2):
        for ch in s.upper():
            g = self.FONT.get(ch, self.FONT[" "])
            for row, line in enumerate(g):
                for col, k in enumerate(line):
                    if k == "X":
                        for dy in range(scale):
                            for dx in range(scale):
                                self.put(x + col * scale + dx, y + row * scale + dy, c)
            x += 6 * scale
        return x

    def text_center(self, y, s, c=FG, scale=2):
        tw = len(s) * 6 * scale
        self.text((self.w - tw) // 2, y, s, c, scale)

    def header(self, current_tab):
        self.rect(0, 0, self.w, 30, PANEL)
        tabs = ["DASH", "CAM", "CTRL", "JOG", "SET"]
        x = 10
        for i, t in enumerate(tabs):
            col = ACCENT if i == current_tab else DIM
            self.text(x, 8, t, col, 2)
            x += 130
        self.rect(620, 10, 10, 10, OK)

    def footer(self, hint):
        self.rect(0, 450, self.w, 30, PANEL)
        self.text(10, 460, hint, DIM, 1)

    def save(self, path):
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "wb") as f:
            f.write(encode_png(self.w, self.h, bytes(self.pix)))


def mockup_dashboard(out):
    c = C(W, H, BG)
    c.header(0)
    c.text_center(50, "PRINTING", OK, 3)
    # Temps
    c.rect(20, 100, 280, 140, PANEL)
    c.text(30, 108, "BUSE", DIM, 1)
    c.text(30, 122, "215 / 215 C", ACCENT, 3)
    c.text(30, 175, "PLATEAU", DIM, 1)
    c.text(30, 187, "60 / 60 C", ACCENT, 3)
    # Position / fan
    c.rect(320, 100, 300, 140, PANEL)
    c.text(330, 108, "POSITION", DIM, 1)
    c.text(330, 125, "X 84  Y 102  Z 12.40", FG, 2)
    c.text(330, 165, "VENTILOS", DIM, 1)
    c.text(330, 180, "HOTEND 100  PRINT 76", FG, 2)
    c.text(330, 210, "FLOW / SPEED", DIM, 1)
    c.text(330, 222, "100% / 100%", FG, 2)
    # Job
    c.rect(20, 260, 600, 150, PANEL)
    c.text(30, 268, "IMPRESSION", DIM, 1)
    c.text(30, 285, "BENCHY_0.2_PLA.GCODE", FG, 2)
    c.rect(30, 340, 580, 24, BG)
    c.rect(30, 340, 232, 24, ACCENT)  # 40%
    c.rect_out(30, 340, 580, 24, FG)
    c.text(30, 372, "40.2%", FG, 2)
    c.text(280, 380, "RESTE 28MIN   /   ECOULE 18MIN", DIM, 1)
    c.footer("L/R: TAB     B: BACK     A: ACTION     SELECT: QUIT")
    c.save(out)


def mockup_webcam(out):
    c = C(W, H, BG)
    c.header(1)
    # Fausse image (degrade + objet en cours)
    c.rect(40, 50, 560, 360, (60, 60, 70, 255))
    # Plateau
    c.rect(80, 350, 480, 30, (140, 140, 150, 255))
    # Objet en construction (un benchy stylise)
    for i in range(30):
        h = 6
        w = 80 - i * 2
        x = (640 - w) // 2
        y = 350 - i * h
        col = (220 - i * 4, 120 - i * 2, 60, 255)
        c.rect(x, y, w, h, col)
    # Tete d'impression
    c.rect(W // 2 - 30, 110, 60, 50, (40, 40, 50, 255))
    c.rect(W // 2 - 1, 160, 2, 100, (240, 80, 60, 255))
    # Cadre
    c.rect_out(40, 50, 560, 360, PANEL, 2)
    # Overlay
    c.text(50, 60, "BUSE 215 C   PLATEAU 60 C", FG, 1)
    c.text(50, 420, "A: RAFRAICHIR MAINTENANT", DIM, 1)
    c.footer("L/R: TAB    A: REFRESH    AUTO REFRESH 5S")
    c.save(out)


def mockup_controls(out):
    c = C(W, H, BG)
    c.header(2)
    c.text_center(40, "CONTROLE DU JOB", ACCENT, 2)
    actions = [
        "PAUSE / REPRENDRE",
        "ANNULER L IMPRESSION",
        "HOME XYZ",
        "BUSE +5 C",
        "BUSE -5 C",
        "PLATEAU +5 C",
    ]
    selected = 0
    for i, t in enumerate(actions):
        y = 80 + i * 54
        bg = ACCENT if i == selected else PANEL
        fg = BG if i == selected else FG
        c.rect(60, y, 520, 44, bg)
        c.text(80, y + 14, t, fg, 2)
    c.text_center(420, "HAUT/BAS: CHOISIR   A: EXECUTER", DIM, 1)
    c.footer("L/R: TAB    A: APPLY    B: BACK")
    c.save(out)


def mockup_jog(out):
    c = C(W, H, BG)
    c.header(3)
    c.text_center(40, "JOG (MANUEL)", ACCENT, 2)
    c.text_center(80, "X 84.0   Y 102.0   Z 12.40", FG, 2)
    c.text_center(110, "PAS: 10 MM  (Y POUR CYCLER)", DIM, 1)
    # Croix XY
    cx, cy, r = 240, 280, 70
    c.rect(cx - r - 25, cy - r - 25, 2 * r + 50, 2 * r + 50, PANEL)
    c.text(cx - r - 18, cy - 18, "X-", FG, 3)
    c.text(cx + r - 18, cy - 18, "X+", FG, 3)
    c.text(cx - 18, cy + r - 18, "Y-", FG, 3)
    c.text(cx - 18, cy - r - 18, "Y+", FG, 3)
    # Z
    c.rect(450, 200, 130, 180, PANEL)
    c.text_center(215, "Z", ACCENT, 2)
    c.text(480, 220, "Z+", FG, 3)
    c.text(480, 320, "Z-", FG, 3)
    c.text(477, 360, "L1 / R1", DIM, 1)
    c.text(20, 420, "D-PAD: XY    L1/R1: Z    A: HOME    Y: PAS", DIM, 1)
    c.footer("L/R: TAB    DPAD: MOVE    A: HOME    Y: STEP")
    c.save(out)


def mockup_settings(out):
    c = C(W, H, BG)
    c.header(4)
    c.text_center(40, "PARAMETRES", ACCENT, 2)
    # URL
    c.rect(20, 80, 600, 50, PANEL)
    c.text(30, 86, "URL PRUSA-LINK", DIM, 1)
    c.text(30, 100, "HTTP://192.168.1.42", FG, 2)
    # API key (masque)
    c.rect(20, 140, 600, 50, PANEL)
    c.text(30, 146, "X-API-KEY", DIM, 1)
    c.text(30, 160, "ABC***Z9", FG, 2)
    # Poll
    c.rect(20, 210, 290, 50, ACCENT)
    c.text(30, 216, "POLL (S)", BG, 1)
    c.text(30, 230, "< 2 S >", BG, 2)
    # Jog
    c.rect(330, 210, 290, 50, PANEL)
    c.text(340, 216, "PAS JOG PAR DEFAUT (MM)", DIM, 1)
    c.text(340, 230, "< 10 MM >", FG, 2)
    # Buttons
    c.rect(20, 280, 290, 44, PANEL)
    c.text(30, 296, "TESTER (A)", FG, 2)
    c.rect(330, 280, 290, 44, ERR)
    c.text(340, 296, "DECONNEXION (A)", FG, 2)
    # Help
    c.text(20, 350, "EDITER APP/PRINTBOY/PRINTER.CFG SUR PC POUR MODIFIER.", DIM, 1)
    c.text(20, 370, "CLE: SETTINGS > NETWORK > PRUSA LINK > API KEY.", DIM, 1)
    c.text(20, 410, "HAUT/BAS: CHOISIR  GAUCHE/DROITE: MODIFIER  A: ACTION", DIM, 1)
    c.footer("L/R: TAB    AUTO-SAVE")
    c.save(out)


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    out = os.path.join(root, "preview")
    print("Generation des mockups PrintBoy...")
    mockup_dashboard(os.path.join(out, "mockup_dashboard.png"))
    print(f"  -> {out}/mockup_dashboard.png")
    mockup_webcam(os.path.join(out, "mockup_webcam.png"))
    print(f"  -> {out}/mockup_webcam.png")
    mockup_controls(os.path.join(out, "mockup_controls.png"))
    print(f"  -> {out}/mockup_controls.png")
    mockup_jog(os.path.join(out, "mockup_jog.png"))
    print(f"  -> {out}/mockup_jog.png")
    mockup_settings(os.path.join(out, "mockup_settings.png"))
    print(f"  -> {out}/mockup_settings.png")
    print("OK - mockups generes (a remplacer par captures reelles)")


if __name__ == "__main__":
    main()
