pub use image::ImageError;

/// CPU image data: tightly packed RGBA8, top row first
pub struct DecodedImage {
    pub width:  u32,
    pub height: u32,
    pub pixels: Vec<u8>,
}

/// Decode an image from its encoded file bytes.
pub fn decode_image(bytes: &[u8]) -> Result<DecodedImage, ImageError> {
    let rgba = image::load_from_memory(bytes)?.into_rgba8();
    let (width, height) = rgba.dimensions();

    Ok(DecodedImage {
        width,
        height,
        pixels: rgba.into_raw()
    })
}
