## Unreleased

- Publish GraphicsFrame through `FramegrabberGraphicsImageStream`; the converter header stays in the adapter translation unit.

- Drain in-flight GraphicsFrame adapter callbacks before stream destruction and cover the shared callback gate contract.
- Move DMA callback registration, ownership return, and GraphicsFrame conversion into the module adapter stream; the parent receives only owned GraphicsFrame values.
- Keep the GraphicsFrame adapter target independent of Qt GUI.
- Keep frame-grabber output on the canonical `GraphicsFrame` host boundary.
- Split the opt-in Qt control panel into `Framegrabber::QtWidget`, leaving the default `Framegrabber::Framegrabber` target free of Qt dependencies.
- Updated the optional scene adapter to consume a neutral scene-contract target without inheriting the visualization runtime; image conversion output is unchanged.
- Replace the corrupted host-layout README with a standalone acquisition contract and correct the buffer pool description.
