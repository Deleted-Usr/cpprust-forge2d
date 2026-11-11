Image loader fixtures:

- `rgba.png`: 2x2 RGBA8; top row red/opaque, green/alpha 64;
  bottom row blue/alpha 0, white/opaque. Tests channel order, alpha and orientation.
- `gray.png`: 2x1 grayscale, values 32 and 192. Tests expansion to RGBA8.
- `red.jpg`: 16x16 solid red JPEG. Tests lossy decoding and opaque alpha.

These are generated test images, independent of the Rust decoder. PNGs use
standard PNG chunks/zlib; the JPEG was encoded with Windows System.Drawing.
