#!/usr/bin/env python3
"""make_icon.py - build the Zanac application icon from the supplied artwork.

The source is `assets/app_icon_source.png`, the player-ship picture used for the
port's icon.  It comes on a black background and is cropped to the ship, whose
black pixels become transparent; the rest is left as drawn (only a mild
saturation/brightness lift, to compensate for the icon being seen at small
sizes).  The ship is then placed on a dark rounded badge with a blue glow and a
sparse starfield, and scaled with nearest-neighbour so its pixels stay square.

Generated files (all committed, so a build does not need Pillow):

    assets/zanac_256.png     preview / documentation
    zanac.ico                Windows icon (16, 24, 32, 48, 64, 128, 256)
    frontend/icon.h          64x64 RGBA, used by SDL_SetWindowIcon()

The .ico uses 32-bit BMP frames up to 64 px (widest shell compatibility) and
PNG frames for 128/256.

Usage:  python3 c_src/tools/make_icon.py
"""
from io import BytesIO
from pathlib import Path
import random
import struct

from PIL import Image, ImageDraw, ImageEnhance, ImageFilter

ROOT = Path(__file__).resolve().parent.parent
ASSETS = ROOT / "assets"
SOURCE = ASSETS / "app_icon_source.png"

SIZES = [16, 24, 32, 48, 64, 128, 256]
# Ship height as a fraction of the icon; grows a little at small sizes so the
# silhouette stays readable in the taskbar.
SHIP_FRACTION = {16: 0.98, 24: 0.92, 32: 0.88, 48: 0.84,
                 64: 0.80, 128: 0.75, 256: 0.75}

BACKGROUND_TOP = (10, 14, 44)
BACKGROUND_BOTTOM = (2, 3, 10)
GLOW = (34, 74, 255)
BORDER = (96, 126, 255)


# --------------------------------------------------------------- source ----
def load_ship() -> Image.Image:
    """Crop the ship out of the supplied picture and key the black out."""
    image = Image.open(SOURCE).convert("RGBA")
    pixels = image.load()
    xs, ys = [], []
    for y in range(image.height):
        for x in range(image.width):
            if max(pixels[x, y][:3]) > 6:        # anything not pure black
                xs.append(x)
                ys.append(y)
    if not xs:
        raise SystemExit(f"{SOURCE}: no ship pixels found")

    ship = image.crop((min(xs), min(ys), max(xs) + 1, max(ys) + 1))
    sp = ship.load()
    alpha = Image.new("L", ship.size, 0)
    ap = alpha.load()
    for y in range(ship.height):
        for x in range(ship.width):
            if max(sp[x, y][:3]) > 6:
                ap[x, y] = 255
    ship.putalpha(alpha)

    # The artwork is a soft, low-resolution capture; a mild lift keeps the
    # red/blue details from turning to mud at 16-32 px.
    ship = ImageEnhance.Brightness(ship).enhance(1.03)
    ship = ImageEnhance.Color(ship).enhance(1.22)
    return ship


# ------------------------------------------------------------ painting -----
def gradient(size: int) -> Image.Image:
    column = Image.new("RGBA", (1, size))
    for y in range(size):
        t = y / max(1, size - 1)
        t = t * t * (3.0 - 2.0 * t)                # smoothstep
        column.putpixel((0, y), tuple(
            round(BACKGROUND_TOP[i] + (BACKGROUND_BOTTOM[i] - BACKGROUND_TOP[i]) * t)
            for i in range(3)) + (255,))
    return column.resize((size, size), Image.BILINEAR)


def add_glow(image: Image.Image, size: int) -> None:
    glow = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(glow)
    cx, cy = size * 0.5, size * 0.47
    rmax = size * 0.55
    for i in range(40):
        r = rmax * (1.0 - i / 40.0)
        a = int(85 * (r / rmax) ** 2)
        draw.ellipse([cx - r, cy - r, cx + r, cy + r], fill=GLOW + (a,))
    image.alpha_composite(glow.filter(ImageFilter.GaussianBlur(size * 0.02)))


def add_stars(image: Image.Image, size: int) -> None:
    if size < 48:
        return
    rng = random.Random(0x5A4EAC)
    draw = ImageDraw.Draw(image)
    for _ in range(max(10, size * size // 600)):
        x, y = rng.randrange(size), rng.randrange(size)
        # Leave the area around the ship fairly clean.
        if (x - size / 2) ** 2 + (y - size / 2) ** 2 < (size * 0.28) ** 2:
            continue
        alpha = rng.choice([40, 60, 90, 130, 170])
        draw.point((x, y), fill=(210, 225, 255, alpha))
        if size >= 128 and rng.random() < 0.2:
            draw.point((x + 1, y), fill=(150, 180, 255, alpha // 2))
            draw.point((x, y + 1), fill=(150, 180, 255, alpha // 2))


def add_border(image: Image.Image, size: int) -> None:
    draw = ImageDraw.Draw(image)
    width = 1 if size <= 64 else 2
    draw.rounded_rectangle(
        [width // 2, width // 2, size - 1 - width // 2, size - 1 - width // 2],
        radius=max(2, round(size * 0.18)), outline=BORDER + (110,), width=width)


def add_ship(image: Image.Image, size: int, ship: Image.Image) -> None:
    height = max(1, round(size * SHIP_FRACTION[size]))
    width = max(1, round(ship.width * height / ship.height))
    sprite = ship.resize((width, height), Image.NEAREST)
    x, y = (size - width) // 2, (size - height) // 2

    # A blue halo behind the sprite (its own silhouette, blurred).
    halo = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    silhouette = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    silhouette.paste(sprite, (x, y))
    tint = Image.new("RGBA", (size, size), (70, 130, 255, 255))
    tint.putalpha(silhouette.getchannel("A"))
    halo.alpha_composite(tint)
    image.alpha_composite(halo.filter(ImageFilter.GaussianBlur(max(1, size * 0.025))))

    image.alpha_composite(sprite, (x, y))


def render(size: int, ship: Image.Image) -> Image.Image:
    image = gradient(size)
    add_glow(image, size)
    add_stars(image, size)

    mask = Image.new("L", (size, size), 0)
    ImageDraw.Draw(mask).rounded_rectangle(
        [0, 0, size - 1, size - 1], radius=max(2, round(size * 0.18)), fill=255)
    image.putalpha(mask)

    add_border(image, size)
    add_ship(image, size, ship)
    return image


# --------------------------------------------------------------- outputs ----
def bmp_frame(image: Image.Image) -> bytes:
    """A 32-bit BMP icon frame: BITMAPINFOHEADER + BGRA + 1-bit AND mask."""
    w, h = image.size
    pixels = image.load()
    xor = bytearray()
    for y in range(h - 1, -1, -1):                 # bottom-up
        for x in range(w):
            r, g, b, a = pixels[x, y]
            xor += bytes((b, g, r, a))
    stride = ((w + 31) // 32) * 4
    mask = bytearray()
    for y in range(h - 1, -1, -1):
        row = bytearray(stride)
        for x in range(w):
            if pixels[x, y][3] == 0:
                row[x // 8] |= 0x80 >> (x % 8)
        mask += row
    header = struct.pack("<IiiHHIIiiII", 40, w, h * 2, 1, 32, 0,
                         len(xor) + len(mask), 0, 0, 0, 0)
    return header + bytes(xor) + bytes(mask)


def write_ico(images: dict[int, Image.Image], path: Path) -> None:
    frames = []
    for size in SIZES:
        image = images[size]
        if size <= 64:
            payload = bmp_frame(image)
        else:
            buffer = BytesIO()
            image.save(buffer, format="PNG")
            payload = buffer.getvalue()
        frames.append((size, payload))

    header = struct.pack("<HHH", 0, 1, len(frames))
    offset = len(header) + 16 * len(frames)
    directory = bytearray()
    for size, payload in frames:
        directory += struct.pack(
            "<BBBBHHII",
            0 if size == 256 else size, 0 if size == 256 else size,
            0, 0, 1, 32, len(payload), offset)
        offset += len(payload)
    path.write_bytes(header + bytes(directory) +
                     b"".join(payload for _, payload in frames))


def write_header(image: Image.Image, path: Path) -> None:
    """A 64x64 RGBA array for SDL_CreateSurfaceFrom()/SDL_SetWindowIcon()."""
    data = image.tobytes()
    lines = [
        "/* icon.h - the application icon as RGBA, for SDL_SetWindowIcon().",
        " *",
        " * GENERATED by c_src/tools/make_icon.py from",
        " * c_src/assets/app_icon_source.png; do not edit.",
        " * Row-major RGBA, 64x64, alpha included. */",
        "#ifndef ZANAC_ICON_RGBA_H",
        "#define ZANAC_ICON_RGBA_H",
        "",
        f"#define ZANAC_ICON_W {image.width}",
        f"#define ZANAC_ICON_H {image.height}",
        "",
        "static const unsigned char zanac_icon_rgba[ZANAC_ICON_W * "
        "ZANAC_ICON_H * 4] = {",
    ]
    for i in range(0, len(data), 12):
        lines.append("    " + ", ".join(f"0x{b:02X}" for b in data[i:i + 12]) + ",")
    lines += ["};", "", "#endif /* ZANAC_ICON_RGBA_H */"]
    path.write_text("\n".join(lines) + "\n", encoding="ascii")


def main() -> None:
    ship = load_ship()
    images = {size: render(size, ship) for size in SIZES}
    ASSETS.mkdir(exist_ok=True)
    (ROOT / "frontend").mkdir(exist_ok=True)

    images[256].save(ASSETS / "zanac_256.png")
    write_ico(images, ROOT / "zanac.ico")
    write_header(images[64], ROOT / "frontend" / "icon.h")
    print(f"source ship: {ship.width}x{ship.height}")
    print("wrote assets/zanac_256.png, zanac.ico, frontend/icon.h")


if __name__ == "__main__":
    main()
