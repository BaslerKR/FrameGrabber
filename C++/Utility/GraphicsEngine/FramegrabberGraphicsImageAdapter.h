#pragma once

#include "Framegrabber.h"
#include "engine/GraphicsFrameAdapter.h"

#include <cstddef>

namespace FramegrabberGraphicsImageAdapter
{
/** Maps an owned Framegrabber image into a GraphicsEngine image value. */
[[nodiscard]] GraphicsImage wrapImage(
    const Framegrabber::Image& image,
    std::size_t frameSeq) noexcept;
}
