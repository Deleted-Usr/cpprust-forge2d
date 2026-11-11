#[cxx::bridge(namespace = "forge::rust")]
mod ffi {
    struct DecodedImage {
        width:  u32,
        height: u32,
        pixels: Vec<u8>,
    }

    extern "Rust" {
        fn decode_image(bytes: &[u8]) -> Result<DecodedImage>;
    }
}

fn decode_image(
    bytes: &[u8],
) -> Result<ffi::DecodedImage, forge_assets::ImageError> {
    let image = forge_assets::decode_image(bytes)?;

    Ok(ffi::DecodedImage {
        width:  image.width,
        height: image.height,
        pixels: image.pixels,
    })
}
