## Unreleased

- Keep frame-grabber output on the canonical `GraphicsFrame` host boundary.
- Split the opt-in Qt control panel into `Framegrabber::QtWidget`, leaving the default `Framegrabber::Framegrabber` target free of Qt dependencies.
- Updated the optional scene adapter to consume a neutral scene-contract target without inheriting the visualization runtime; image conversion output is unchanged.
- Replace the corrupted host-layout README with a standalone acquisition contract and correct the buffer pool description.
