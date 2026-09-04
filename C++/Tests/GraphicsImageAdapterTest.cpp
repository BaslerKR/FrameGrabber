#include "FramegrabberGraphicsImageAdapter.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

int main()
{
    auto storage = std::make_shared<std::vector<std::uint8_t>>(4U);
    (*storage)[0] = 1U;
    (*storage)[3] = 4U;

    Framegrabber::Image source;
    source.storage = std::shared_ptr<const std::uint8_t>(storage, storage->data());
    source.size = storage->size();
    source.width = 2;
    source.height = 2;
    source.stride = 2;
    source.bitsPerPixel = 8;
    source.pixelFormat = Framegrabber::PixelFormat::Mono8;
    source.bitAlignment = Framegrabber::BitAlignment::Packed;

    const GraphicsImage image = FramegrabberGraphicsImageAdapter::wrapImage(source, 9U);
    assert(image.isValid());
    assert(image.pixelFormat == GraphicsImagePixelFormat::Mono8);
    assert(image.frameSequence == 9U);
    assert(image.data() == storage->data());
    assert(image.data()[3] == 4U);
    return 0;
}
