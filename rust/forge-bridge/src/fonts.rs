#[cxx::bridge(namespace = "forge::rust")]
mod ffi {
    struct GlyphPlacement {
        index:  u16,
        x:      f32,
        y:      f32,
        width:  u32,
        height: u32,
    }
    
    struct TextLayout {
        glyphs: Vec<GlyphPlacement>,
        width:  f32,
        height: f32,
    }
    
    struct GlyphBitmap {
        width:  u32,
        height: u32,
        pixels: Vec<u8>,
    }
    
    extern "Rust" {
        type FontFace;
        fn create_font_face(bytes: &[u8]) -> Result<Box<FontFace>>;
        fn layout(self: &FontFace, bytes: &[u8], size: f32) -> Result<TextLayout>;
        fn rasterise(self: &FontFace, index: u16, size: f32) -> Result<GlyphBitmap>;
    }
}

pub struct FontFace {
    inner: forge_font::FontFace,
}

// CXX translates errors to C++ exceptions. Catch parser/rasteriser panics too;
// unwinding must never cross the bridge. Allocation aborts remain process-fatal.
fn guarded<T>(f: impl FnOnce() -> Result<T, String>) -> Result<T, String> {
    std::panic::catch_unwind(std::panic::AssertUnwindSafe(f))
        .map_err(|_| "Font operation panicked".to_owned())?
}

fn create_font_face(bytes: &[u8]) -> Result<Box<FontFace>, String> {
    guarded(|| {
        Ok(Box::new(FontFace {
            inner: forge_font::FontFace::from_bytes(bytes)?,
        }))
    })
}

impl FontFace {
    fn layout(&self, bytes: &[u8], size: f32) -> Result<ffi::TextLayout, String> {
        guarded(|| {
            let result = self.inner.layout(bytes, size)?;
            
            Ok(ffi::TextLayout {
                width: result.width,
                height: result.height,
                glyphs: result
                    .glyphs
                    .into_iter()
                    .map(|g| ffi::GlyphPlacement {
                        index: g.index,
                        x: g.x,
                        y: g.y,
                        width: g.width,
                        height: g.height,
                    })
                    .collect(),
            })
        })
    }
    fn rasterise(&self, index: u16, size: f32) -> Result<ffi::GlyphBitmap, String> {
        guarded(|| {
            let (m, pixels) = self.inner.rasterise(index, size)?;
            
            Ok(ffi::GlyphBitmap {
                width: m.width as u32,
                height: m.height as u32,
                pixels,
            })
        })
    }
}
