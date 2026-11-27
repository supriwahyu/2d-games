#!/usr/bin/env python3
"""Generate crisp pixel-art sprites and tiles for the Mario-style Godot 2D game."""
import os
from PIL import Image, ImageDraw

OUT_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), "assets")
os.makedirs(OUT_DIR, exist_ok=True)


def save_scaled(img: Image.Image, name: str, scale: int = 2) -> None:
    if scale > 1:
        img = img.resize((img.width * scale, img.height * scale), Image.NEAREST)
    img.save(os.path.join(OUT_DIR, name))


def draw_grid(grid, palette):
    h = len(grid)
    w = len(grid[0])
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    px = img.load()
    for y, row in enumerate(grid):
        for x, ch in enumerate(row):
            if ch in palette:
                px[x, y] = palette[ch]
    return img


def make_player():
    pal = {
        "R": (220, 40, 40, 255),    # Red cap & shirt
        "S": (255, 200, 150, 255),  # Skin
        "H": (110, 60, 20, 255),    # Hair/mustache/boots
        "B": (40, 90, 220, 255),    # Blue overalls
        "Y": (255, 220, 50, 255),   # Buttons
        "E": (20, 20, 20, 255),     # Eyes
    }
    idle = [
        "....RRRRRR......",
        "...RRRRRRRRR....",
        "...HHHSSES......",
        "..HSHSSSESSS....",
        "..HSHHSSSSHHH...",
        "...HSSSSHHHH....",
        "....RRRRRRR.....",
        "...RRBRRRBRR....",
        "..RRRBRRRBRRR...",
        "..SS.BYBBYB.SS..",
        "..SS.BBBBBB.SS..",
        ".....BBBBBB.....",
        "....BBB..BBB....",
        "....HHH..HHH....",
        "...HHHH..HHHH...",
        "................",
    ]
    run1 = [
        "....RRRRRR......",
        "...RRRRRRRRR....",
        "...HHHSSES......",
        "..HSHSSSESSS....",
        "..HSHHSSSSHHH...",
        "...HSSSSHHHH....",
        "....RRRRRRR.....",
        "...RRBRRRBRR....",
        "..SS.BRRRBRR.SS.",
        ".....BYBBYB.....",
        "....BBBBBBBB....",
        "....BBB..BBB....",
        "...HHH....HHH...",
        "..HHHH.....HHH..",
        "................",
        "................",
    ]
    run2 = [
        "....RRRRRR......",
        "...RRRRRRRRR....",
        "...HHHSSES......",
        "..HSHSSSESSS....",
        "..HSHHSSSSHHH...",
        "...HSSSSHHHH....",
        "....RRRRRRR.....",
        "...RRBRRRBRR....",
        "....RBRRRBRR....",
        "....SBYBBYBS....",
        ".....BBBBBB.....",
        ".....BBBBB......",
        ".....HHH.HHH....",
        "....HHHH..HHH...",
        "................",
        "................",
    ]
    jump = [
        "....RRRRRR...SS.",
        "...RRRRRRRRR.SS.",
        "...HHHSSES..RRR.",
        "..HSHSSSESSSRR..",
        "..HSHHSSSSHHH...",
        "...HSSSSHHHH....",
        "....RRRRRRR.....",
        "..SSRRBRRRBR....",
        "....RBRRRBR.....",
        "....BYBBYB......",
        "...BBBBBBBB.....",
        "..HHH....BBB....",
        "..HHH.....HHH...",
        "..........HHH...",
        "................",
        "................",
    ]
    save_scaled(draw_grid(idle, pal), "player_idle.png", scale=2)
    save_scaled(draw_grid(run1, pal), "player_run1.png", scale=2)
    save_scaled(draw_grid(run2, pal), "player_run2.png", scale=2)
    save_scaled(draw_grid(jump, pal), "player_jump.png", scale=2)


def make_enemy():
    pal = {
        "B": (155, 82, 35, 255),
        "D": (95, 45, 15, 255),
        "S": (245, 210, 170, 255),
        "W": (255, 255, 255, 255),
        "K": (20, 20, 20, 255),
    }
    walk1 = [
        "......BBBB......",
        "....BBBBBBBB....",
        "...BBBBBBBBBB...",
        "..BBBBBBBBBBBB..",
        ".BBBBWBBBBWBBBB.",
        ".BBBKWKBBKWKBBB.",
        ".BBBKWKBBKWKBBB.",
        ".BBBBBBBBBBBBBB.",
        "..DDDDSSSSDDDD..",
        "....SSSSSSSS....",
        "...SSSSSSSSSS...",
        "....KKKKKKKK....",
        "...KKKK..KKKK...",
        "..KKKK....KKKK..",
        "................",
        "................",
    ]
    walk2 = [
        "......BBBB......",
        "....BBBBBBBB....",
        "...BBBBBBBBBB...",
        "..BBBBBBBBBBBB..",
        ".BBBBWBBBBWBBBB.",
        ".BBBKWKBBKWKBBB.",
        ".BBBKWKBBKWKBBB.",
        ".BBBBBBBBBBBBBB.",
        "..DDDDSSSSDDDD..",
        "....SSSSSSSS....",
        "...SSSSSSSSSS...",
        "....KKKKKKKK....",
        "....KKKK.KKKK...",
        ".....KKK..KKK...",
        "................",
        "................",
    ]
    squashed = [
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "..BBBBBBBBBBBB..",
        ".BBBKWKBBKWKBBB.",
        ".BBBBBBBBBBBBBB.",
        "..DDDDSSSSDDDD..",
        ".KKKKKKKKKKKKKK.",
        "................",
        "................",
    ]
    save_scaled(draw_grid(walk1, pal), "enemy_walk1.png", scale=2)
    save_scaled(draw_grid(walk2, pal), "enemy_walk2.png", scale=2)
    save_scaled(draw_grid(squashed, pal), "enemy_squashed.png", scale=2)


def make_coin():
    pal = {
        "Y": (255, 215, 30, 255),
        "L": (255, 245, 140, 255),
        "O": (210, 140, 15, 255),
        "B": (140, 85, 10, 255),
    }
    coin = [
        "...BBBBBB...",
        "..BYYYYYYB..",
        ".BYYLLLLYYB.",
        ".BYLYOOYLYB.",
        ".BYLYOOYLYB.",
        ".BYLYOOYLYB.",
        ".BYLYOOYLYB.",
        ".BYLYOOYLYB.",
        ".BYYLLLLYYB.",
        "..BYYYYYYB..",
        "...BBBBBB...",
        "............",
    ]
    save_scaled(draw_grid(coin, pal), "coin.png", scale=2)


def make_blocks():
    # Ground block 32x32
    g = Image.new("RGBA", (32, 32), (168, 92, 48, 255))
    d = ImageDraw.Draw(g)
    # Grass top
    d.rectangle([0, 0, 31, 7], fill=(60, 185, 65, 255))
    d.rectangle([0, 0, 31, 2], fill=(105, 225, 95, 255))
    d.rectangle([0, 7, 31, 8], fill=(35, 120, 40, 255))
    # Dirt speckles
    for x, y in [(4, 12), (14, 15), (24, 11), (8, 22), (20, 25), (27, 20)]:
        d.rectangle([x, y, x + 3, y + 2], fill=(130, 68, 32, 255))
        d.rectangle([x + 1, y + 1, x + 2, y + 1], fill=(200, 125, 75, 255))
    g.save(os.path.join(OUT_DIR, "block_ground.png"))

    # Brick block 32x32
    b = Image.new("RGBA", (32, 32), (185, 80, 40, 255))
    d = ImageDraw.Draw(b)
    mortar = (60, 30, 20, 255)
    hi = (225, 115, 65, 255)
    for y in [0, 8, 16, 24]:
        d.line([0, y + 7, 31, y + 7], fill=mortar)
        d.line([0, y, 31, y], fill=hi)
    for x in [15, 31]:
        d.line([x, 0, x, 7], fill=mortar)
        d.line([x, 16, x, 23], fill=mortar)
    for x in [7, 23]:
        d.line([x, 8, x, 15], fill=mortar)
        d.line([x, 24, x, 31], fill=mortar)
    b.save(os.path.join(OUT_DIR, "block_brick.png"))

    # Question block 32x32
    q = Image.new("RGBA", (32, 32), (245, 180, 35, 255))
    d = ImageDraw.Draw(q)
    d.rectangle([0, 0, 31, 31], outline=(140, 80, 15, 255), width=2)
    d.rectangle([2, 2, 29, 29], outline=(255, 225, 110, 255), width=1)
    # Corner rivets
    for rx, ry in [(4, 4), (26, 4), (4, 26), (26, 26)]:
        d.rectangle([rx, ry, rx + 1, ry + 1], fill=(140, 80, 15, 255))
    # '?' glyph
    qm_shadow = (140, 70, 10, 255)
    qm_fg = (255, 245, 200, 255)
    for dx, dy, col in [(1, 1, qm_shadow), (0, 0, qm_fg)]:
        d.rectangle([11 + dx, 7 + dy, 20 + dx, 9 + dy], fill=col)
        d.rectangle([9 + dx, 9 + dy, 12 + dx, 12 + dy], fill=col)
        d.rectangle([18 + dx, 9 + dy, 21 + dx, 14 + dy], fill=col)
        d.rectangle([14 + dx, 14 + dy, 18 + dx, 17 + dy], fill=col)
        d.rectangle([14 + dx, 17 + dy, 17 + dx, 20 + dy], fill=col)
        d.rectangle([14 + dx, 22 + dy, 17 + dx, 25 + dy], fill=col)
    q.save(os.path.join(OUT_DIR, "block_question.png"))

    # Used block 32x32
    u = Image.new("RGBA", (32, 32), (135, 105, 85, 255))
    d = ImageDraw.Draw(u)
    d.rectangle([0, 0, 31, 31], outline=(75, 55, 45, 255), width=2)
    for rx, ry in [(4, 4), (26, 4), (4, 26), (26, 26)]:
        d.rectangle([rx, ry, rx + 1, ry + 1], fill=(75, 55, 45, 255))
    u.save(os.path.join(OUT_DIR, "block_used.png"))


def make_scenery():
    # Pipe 48x64
    p = Image.new("RGBA", (48, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(p)
    # Pipe body
    d.rectangle([4, 18, 43, 63], fill=(40, 175, 50, 255), outline=(15, 70, 20, 255), width=2)
    d.rectangle([10, 19, 16, 62], fill=(120, 235, 120, 255))
    d.rectangle([34, 19, 41, 62], fill=(25, 120, 35, 255))
    # Pipe lip
    d.rectangle([0, 0, 47, 19], fill=(40, 175, 50, 255), outline=(15, 70, 20, 255), width=2)
    d.rectangle([7, 2, 14, 17], fill=(120, 235, 120, 255))
    d.rectangle([36, 2, 45, 17], fill=(25, 120, 35, 255))
    p.save(os.path.join(OUT_DIR, "pipe.png"))

    # Flagpole 32x160
    f = Image.new("RGBA", (32, 160), (0, 0, 0, 0))
    d = ImageDraw.Draw(f)
    d.rectangle([14, 12, 17, 159], fill=(230, 230, 230, 255), outline=(90, 90, 90, 255), width=1)
    d.ellipse([10, 2, 21, 13], fill=(255, 215, 40, 255), outline=(140, 100, 10, 255))
    # Flag
    d.polygon([(18, 16), (31, 24), (18, 34)], fill=(230, 45, 45, 255))
    f.save(os.path.join(OUT_DIR, "flagpole.png"))

    # Castle 96x96
    c = Image.new("RGBA", (96, 96), (0, 0, 0, 0))
    d = ImageDraw.Draw(c)
    stone = (185, 115, 82, 255)
    stone_hi = (210, 140, 102, 255)
    dark = (95, 55, 40, 255)
    # Main keep
    d.rectangle([12, 36, 83, 95], fill=stone, outline=dark, width=2)
    d.line([14, 38, 81, 38], fill=stone_hi)
    # Top battlements
    for bx in range(12, 84, 16):
        d.rectangle([bx, 26, bx + 10, 36], fill=stone, outline=dark, width=2)
    # Center tower
    d.rectangle([28, 16, 67, 36], fill=stone, outline=dark, width=2)
    for bx in range(28, 68, 14):
        d.rectangle([bx, 8, bx + 9, 16], fill=stone, outline=dark, width=2)
    # Castle roof pennant
    d.line([47, 0, 47, 8], fill=(230, 230, 230, 255))
    d.polygon([(48, 0), (60, 4), (48, 8)], fill=(235, 50, 50, 255))
    # Door
    d.rectangle([38, 64, 57, 95], fill=(35, 25, 25, 255))
    d.pieslice([38, 54, 57, 74], 180, 360, fill=(35, 25, 25, 255))
    # Windows with warm torchlight glow
    d.rectangle([22, 48, 30, 58], fill=(255, 195, 70, 255), outline=(35, 25, 25, 255))
    d.rectangle([65, 48, 73, 58], fill=(255, 195, 70, 255), outline=(35, 25, 25, 255))
    c.save(os.path.join(OUT_DIR, "castle.png"))

    # Cloud 64x32
    cl = Image.new("RGBA", (64, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(cl)
    # Soft blue-tinted shadow base
    d.ellipse([8, 12, 32, 31], fill=(210, 230, 250, 240))
    d.ellipse([20, 6, 46, 31], fill=(210, 230, 250, 240))
    d.ellipse([34, 12, 58, 31], fill=(210, 230, 250, 240))
    d.rectangle([14, 20, 52, 31], fill=(210, 230, 250, 240))
    # Bright white top
    d.ellipse([8, 9, 32, 27], fill=(255, 255, 255, 245))
    d.ellipse([20, 3, 46, 27], fill=(255, 255, 255, 245))
    d.ellipse([34, 9, 58, 27], fill=(255, 255, 255, 245))
    d.rectangle([14, 16, 52, 27], fill=(255, 255, 255, 245))
    cl.save(os.path.join(OUT_DIR, "cloud.png"))

    # Rolling green hill 128x64
    h = Image.new("RGBA", (128, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(h)
    d.pieslice([0, 4, 127, 124], 180, 360, fill=(52, 168, 72, 255), outline=(28, 105, 42, 255), width=2)
    d.arc([4, 8, 123, 120], 195, 345, fill=(105, 215, 105, 255), width=3)
    for sx, sy, sw, sh in [(36, 34, 10, 14), (72, 26, 12, 16), (92, 42, 8, 10)]:
        d.ellipse([sx, sy, sx + sw, sy + sh], fill=(35, 125, 50, 255))
    h.save(os.path.join(OUT_DIR, "hill.png"))

    # Lush bush 96x32
    bu = Image.new("RGBA", (96, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(bu)
    d.ellipse([4, 10, 36, 31], fill=(40, 165, 60, 255), outline=(20, 95, 35, 255), width=2)
    d.ellipse([58, 10, 90, 31], fill=(40, 165, 60, 255), outline=(20, 95, 35, 255), width=2)
    d.ellipse([24, 2, 70, 31], fill=(55, 190, 75, 255), outline=(20, 95, 35, 255), width=2)
    d.rectangle([16, 20, 78, 31], fill=(40, 165, 60, 255))
    d.ellipse([32, 6, 56, 18], fill=(115, 230, 110, 255))
    bu.save(os.path.join(OUT_DIR, "bush.png"))

    # Colorful flower patch 32x16
    fl = Image.new("RGBA", (32, 16), (0, 0, 0, 0))
    d = ImageDraw.Draw(fl)
    flowers = [
        (5, 4, (245, 65, 85, 255)),    # Red/coral
        (15, 2, (255, 220, 50, 255)),  # Yellow
        (25, 5, (235, 110, 220, 255)), # Pink/magenta
    ]
    for fx, fy, fcol in flowers:
        d.line([fx, fy + 3, fx, 15], fill=(40, 155, 50, 255), width=2)
        d.rectangle([fx - 2, fy + 6, fx - 1, fy + 7], fill=(75, 200, 75, 255))
        d.ellipse([fx - 3, fy - 2, fx + 3, fy + 4], fill=fcol)
        d.point((fx, fy + 1), fill=(255, 250, 200, 255))
    fl.save(os.path.join(OUT_DIR, "flowers.png"))


def _make_round_btn(size, fill, border, glyph_fn):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    pad = 3
    d.rounded_rectangle([pad, pad, size - 1 - pad, size - 1 - pad], radius=24, fill=fill, outline=border, width=3)
    glyph_fn(d, size)
    return img


def _make_circle_btn(size, fill, border, glyph_fn):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    pad = 3
    d.ellipse([pad, pad, size - 1 - pad, size - 1 - pad], fill=fill, outline=border, width=3)
    glyph_fn(d, size)
    return img


def make_mobile_buttons():
    def draw_left(d, s):
        m = s // 2
        d.polygon([(m - 22, m), (m + 14, m - 24), (m + 14, m + 24)], fill=(255, 255, 255, 235))

    def draw_right(d, s):
        m = s // 2
        d.polygon([(m + 22, m), (m - 14, m - 24), (m - 14, m + 24)], fill=(255, 255, 255, 235))

    def draw_jump(d, s):
        m = s // 2
        d.polygon([(m, m - 24), (m - 24, m + 10), (m + 24, m + 10)], fill=(255, 250, 210, 245))
        d.rectangle([m - 9, m + 8, m + 9, m + 24], fill=(255, 250, 210, 245))

    def draw_run(d, s):
        m = s // 2
        d.polygon([(m - 2, m), (m - 22, m - 16), (m - 22, m + 16)], fill=(255, 235, 235, 240))
        d.polygon([(m + 22, m), (m + 2, m - 16), (m + 2, m + 16)], fill=(255, 235, 235, 240))

    dpad_specs = [
        ("btn_left", 116, (20, 32, 68, 130), (110, 195, 255, 200), (45, 85, 165, 210), draw_left),
        ("btn_right", 116, (20, 32, 68, 130), (110, 195, 255, 200), (45, 85, 165, 210), draw_right),
    ]
    for name, sz, bg, border, bg_pressed, fn in dpad_specs:
        _make_round_btn(sz, bg, border, fn).save(os.path.join(OUT_DIR, f"{name}.png"))
        _make_round_btn(sz, bg_pressed, (255, 255, 255, 245), fn).save(os.path.join(OUT_DIR, f"{name}_pressed.png"))

    action_specs = [
        ("btn_jump", 124, (45, 35, 15, 135), (255, 215, 55, 215), (210, 155, 25, 220), draw_jump),
        ("btn_run", 96, (60, 20, 28, 135), (255, 110, 110, 210), (195, 55, 65, 215), draw_run),
    ]
    for name, sz, bg, border, bg_pressed, fn in action_specs:
        _make_circle_btn(sz, bg, border, fn).save(os.path.join(OUT_DIR, f"{name}.png"))
        _make_circle_btn(sz, bg_pressed, (255, 255, 255, 245), fn).save(os.path.join(OUT_DIR, f"{name}_pressed.png"))

    # Restart pill button (180x56)
    for suffix, bg, border in [
        ("", (30, 120, 60, 215), (140, 245, 150, 245)),
        ("_pressed", (55, 175, 90, 240), (255, 255, 255, 255)),
    ]:
        r = Image.new("RGBA", (180, 56), (0, 0, 0, 0))
        d = ImageDraw.Draw(r)
        d.rounded_rectangle([2, 2, 177, 53], radius=14, fill=bg, outline=border, width=3)
        # Circular replay arrow icon in center
        d.arc([72, 12, 104, 44], 35, 325, fill=(255, 255, 255, 245), width=4)
        d.polygon([(108, 16), (95, 12), (102, 25)], fill=(255, 255, 255, 245))
        r.save(os.path.join(OUT_DIR, f"btn_restart{suffix}.png"))


if __name__ == "__main__":
    make_player()
    make_enemy()
    make_coin()
    make_blocks()
    make_scenery()
    make_mobile_buttons()
    print("Generated assets in", OUT_DIR)
