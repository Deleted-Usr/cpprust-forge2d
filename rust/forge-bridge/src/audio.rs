#[cxx::bridge(namespace = "forge::rust")]
mod ffi {
    extern "Rust" {
        // Expose an opaque Rust type to C++. C++ can own it and call its methods,
        // but cannot inspect its fields.
        type AudioEngine;

        fn create_audio_engine() -> Result<Box<AudioEngine>>;

        fn load_sound(
            self: &mut AudioEngine,
            path: &str,
        ) -> Result<usize>;

        fn play_sound(
            self: &mut AudioEngine,
            id:   usize,
        ) -> Result<()>;
    }
}

/// Bridge-owned wrapper around the actual audio system.
pub struct AudioEngine {
    inner: forge_audio::AudioEngine,
}

fn create_audio_engine() -> Result<Box<AudioEngine>, String> {
    let inner = forge_audio::AudioEngine::new()?;

    Ok(Box::new(AudioEngine { inner }))
}

impl AudioEngine {
    fn load_sound(&mut self, path: &str) -> Result<usize, String> {
        self.inner.load_sound(path)
    }

    fn play_sound(&mut self, id: usize) -> Result<(), String> {
        self.inner.play_sound(id)
    }
}
