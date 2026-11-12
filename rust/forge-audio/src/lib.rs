use kira::{
    AudioManager,
    AudioManagerSettings,
    DefaultBackend,
    sound::static_sound::StaticSoundData,
};

pub struct AudioEngine {
    manager: AudioManager<DefaultBackend>,
    sounds:  Vec<StaticSoundData>,
}

impl AudioEngine {
    pub fn new() -> Result<Self, String> {
        let manager = AudioManager::<DefaultBackend>::new(
            AudioManagerSettings::default(),
        ).map_err(|error| error.to_string())?;

        Ok(Self {
            manager,
            sounds: Vec::new(),
        })
    }

    /// Loads a short sound into memory
    ///
    /// The returned ID belongs to this AudioEngine instance,
    pub fn load_sound(&mut self, path: &str) -> Result<usize, String> {
        let sound = StaticSoundData::from_file(path)
            .map_err(|error| error.to_string())?;

        let id = self.sounds.len();
        self.sounds.push(sound);

        Ok(id)
    }

    /// Plays an already loaded sound
    pub fn play_sound(&mut self, id: usize) -> Result<(), String> {
        let sound = self.sounds
            .get(id)
            .ok_or_else(|| format!("Invalid sound ID: {id}"))?
            .clone();

        self.manager
            .play(sound)
            .map_err(|error| error.to_string())?;

        Ok(())
    }
}
