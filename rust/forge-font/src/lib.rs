//! CPU-only font parsing, rasterisation and basic left-to-right layout.
use fontdue::{Font, FontSettings, Metrics};

pub const MAX_PIXEL_SIZE: f32 = 512.0;

pub struct FontFace {
    font: Font,
}

pub struct PositionedGlyph {
    pub index:  u16,
    pub x:      f32,
    pub y:      f32,
    pub width:  u32,
    pub height: u32,
}

pub struct TextLayout {
    pub glyphs: Vec<PositionedGlyph>,
    pub width:  f32,
    pub height: f32,
}

fn validate_size(size: f32) -> Result<(), String> {
    if !size.is_finite() || size <= 0.0 || size > MAX_PIXEL_SIZE {
        return Err("Font size must be finite, positive and at most 512 pixels per em".into());
    }

    Ok(())
}

impl FontFace {
    pub fn from_bytes(bytes: &[u8]) -> Result<Self, String> {
        let font = Font::from_bytes(bytes, FontSettings::default()).map_err(str::to_owned)?;
        Ok(Self { font })
    }

    pub fn rasterise(&self, index: u16, size: f32) -> Result<(Metrics, Vec<u8>), String> {
        validate_size(size)?;

        if index >= self.font.glyph_count() {
            return Err("Invalid glyph index".into());
        }

        let metrics = self.font.metrics_indexed(index, size);
        if metrics.width > 4096 || metrics.height > 4096 {
            return Err("Rasterised glyph exceeds 4096 pixels in one dimension".into());
        }

        Ok(self.font.rasterize_indexed(index, size))
    }

    /// Position is the top-left of a typographic line box. Glyphs may overhang it.
    /// Malformed UTF-8 becomes U+FFFD. This is not a shaping/bidi implementation.
    pub fn layout(&self, bytes: &[u8], size: f32) -> Result<TextLayout, String> {
        validate_size(size)?;

        let mut output = TextLayout {
            glyphs: Vec::new(),
            width: 0.0,
            height: 0.0,
        };

        if bytes.is_empty() {
            return Ok(output);
        }

        let lines = self
            .font
            .horizontal_line_metrics(size)
            .ok_or("Font has no horizontal line metrics")?;

        if !lines.ascent.is_finite()        ||
           !lines.new_line_size.is_finite() ||
            lines.new_line_size <= 0.0
        {
            return Err("Font has invalid horizontal line metrics".into());
        }

        let text       = String::from_utf8_lossy(bytes);
        let mut chars = text.chars().peekable();

        let mut x = 0.0_f32;
        let mut baseline = lines.ascent;

        let mut previous = None;

        let tab_width = (self.font.metrics(' ', size).advance_width * 4.0).max(1.0);

        output.height = lines.new_line_size;
        while let Some(character) = chars.next() {
            match character {
                '\r' | '\n' => {
                    if character == '\r' && chars.peek() == Some(&'\n') {
                        chars.next();
                    }

                    output.width = output.width.max(x);

                    x = 0.0;

                    baseline      += lines.new_line_size;
                    output.height += lines.new_line_size;

                    previous = None;
                    continue;
                }
                '\t' => {
                    x = ((x / tab_width).floor() + 1.0) * tab_width;

                    previous = None;
                    continue;
                }
                c if c.is_control() => {
                    previous = None;
                    continue;
                }
                _ => {}
            }

            // Missing codepoints consistently use the font's .notdef glyph.
            let index = self.font.lookup_glyph_index(character);

            if let Some(left) = previous {
                x += self
                    .font
                    .horizontal_kern_indexed(left, index, size)
                    .unwrap_or(0.0);
            }

            let m = self.font.metrics_indexed(index, size);
            if m.width != 0 && m.height != 0 {
                output.glyphs.push(PositionedGlyph {
                    index,
                    x: x + m.xmin as f32,
                    y: baseline - (m.ymin as f32 + m.height as f32),
                    width:  m.width  as u32,
                    height: m.height as u32,
                });
            }

            x += m.advance_width;
            previous = Some(index);
        }

        output.width = output.width.max(x);
        Ok(output)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    const FONT: &[u8] = include_bytes!("../../../tests/fixtures/fonts/Ubuntu-Light.ttf");
    #[test]
    fn metrics_layout_and_rasterisation() {
        let face   = FontFace::from_bytes(FONT).unwrap();
        let layout = face.layout("AV g\n\u{e9}".as_bytes(), 32.0).unwrap();

        assert_eq!(layout.glyphs.len(), 4);
        assert!(layout.height > 32.0);

        let a = &layout.glyphs[0];
        let (m, pixels) = face.rasterise(a.index, 32.0).unwrap();

        assert_eq!(pixels.len(), m.width * m.height);
        assert!(pixels.iter().any(|p| *p > 0));
        assert_eq!(m.width as u32, a.width);

        let unkerned = face.font.metrics('A', 32.0).advance_width + face.font.metrics('V', 32.0).advance_width;
        let kern     = face.font.horizontal_kern('A', 'V', 32.0).unwrap_or(0.0);

        assert!((face.layout(b"AV", 32.0).unwrap().width - (unkerned + kern)).abs() < 0.0001);
    }

    #[test]
    fn whitespace_utf8_and_validation() {
        let face = FontFace::from_bytes(FONT).unwrap();
        
        assert!(face.layout(b" \t\n", 24.0).unwrap().glyphs.is_empty());
        assert_eq!(face.layout(b"", 24.0).unwrap().height, 0.0);
        assert_eq!(
            face.layout(b"a\r\nb", 24.0).unwrap().height,
            face.layout(b"a\nb", 24.0).unwrap().height
        );
        
        assert_eq!(face.layout(b"\xff", 24.0).unwrap().glyphs.len(), 1);
        for size in [0.0, -1.0, f32::NAN, f32::INFINITY, 513.0] {
            assert!(face.layout(b"A", size).is_err());
            assert!(face.rasterise(1, size).is_err());
        }
        
        assert!(FontFace::from_bytes(b"not a font").is_err());
    }
}
