//! Modules are included here to expose functions C++.
//!
//! Each module gets its own `.rs` wrapper next to this one. Each declaration
//! tells rust to load a module from its respective declaration, and those
//! files contain exported wrappers; the actual sub-system logic lives
//! elsewhere.
//!
//! The useful distinction is:
//! | Addition                                    | Where it goes                            |
//! |---------------------------------------------|------------------------------------------|
//! | The sub-system's Rust implementation        | A module or crate such as `forge-assets` |
//! | Functions that let C++ call that sub-system | A module inside `forge-bridge`           |
mod assets;
mod audio;
