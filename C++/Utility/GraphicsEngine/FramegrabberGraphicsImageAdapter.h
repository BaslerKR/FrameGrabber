#pragma once

#include "Framegrabber.h"
#include "engine/GraphicsFrameAdapter.h"

#include <cstddef>

namespace FramegrabberGraphicsImageAdapter
{
[[nodiscard]] GraphicsImage wrapImage(
    const Framegrabber::Image& image,
    std::size_t frameSeq) noexcept;

/** Owns frame-grabber callback registration and emits only owned GraphicsFrame values. */
class GraphicsImageFrameStream final
{
public:
    GraphicsImageFrameStream(Framegrabber* framegrabber, GraphicsFrameCallback callback);
    ~GraphicsImageFrameStream();

    GraphicsImageFrameStream(const GraphicsImageFrameStream&) = delete;
    GraphicsImageFrameStream& operator=(const GraphicsImageFrameStream&) = delete;

private:
    Framegrabber* _framegrabber = nullptr;
    GraphicsFrameCallback _callback;
    Framegrabber::CallbackId _grabCallbackId = 0;
};
}
