#include "FramegrabberGraphicsImageAdapter.h"
#include "FramegrabberSystem.h"

#include <exception>
#include <utility>

namespace
{
[[nodiscard]] GraphicsImagePixelFormat graphicsPixelFormat(
    const Framegrabber::PixelFormat format) noexcept
{
#define MAP_PIXEL_FORMAT(name) \
    case Framegrabber::PixelFormat::name: return GraphicsImagePixelFormat::name
    switch (format)
    {
    MAP_PIXEL_FORMAT(Unknown);
    MAP_PIXEL_FORMAT(Mono8);
    MAP_PIXEL_FORMAT(Mono10Packed);
    MAP_PIXEL_FORMAT(Mono12Packed);
    MAP_PIXEL_FORMAT(Mono14Packed);
    MAP_PIXEL_FORMAT(Mono16);
    MAP_PIXEL_FORMAT(Mono32);
    MAP_PIXEL_FORMAT(Binary);
    MAP_PIXEL_FORMAT(RGB24);
    MAP_PIXEL_FORMAT(BGR24);
    MAP_PIXEL_FORMAT(YCbCr422_8);
    MAP_PIXEL_FORMAT(YCbCr422_8_CbYCrY);
    MAP_PIXEL_FORMAT(RGBA32);
    MAP_PIXEL_FORMAT(RGB30);
    MAP_PIXEL_FORMAT(RGB48);
    MAP_PIXEL_FORMAT(BGR48);
    MAP_PIXEL_FORMAT(RGB10Packed);
    MAP_PIXEL_FORMAT(RGB12Packed);
    MAP_PIXEL_FORMAT(RGB14Packed);
    MAP_PIXEL_FORMAT(BGR10Packed);
    MAP_PIXEL_FORMAT(BGR12Packed);
    MAP_PIXEL_FORMAT(BGR14Packed);
    MAP_PIXEL_FORMAT(RGBA10Packed);
    MAP_PIXEL_FORMAT(RGBA12Packed);
    MAP_PIXEL_FORMAT(RGBA14Packed);
    MAP_PIXEL_FORMAT(RGBA64);
    MAP_PIXEL_FORMAT(BGRA32);
    MAP_PIXEL_FORMAT(BGRA10Packed);
    MAP_PIXEL_FORMAT(BGRA12Packed);
    MAP_PIXEL_FORMAT(BGRA14Packed);
    MAP_PIXEL_FORMAT(BGRA64);
    MAP_PIXEL_FORMAT(RGBX32);
    MAP_PIXEL_FORMAT(RGBX10Packed);
    MAP_PIXEL_FORMAT(RGBX12Packed);
    MAP_PIXEL_FORMAT(RGBX14Packed);
    MAP_PIXEL_FORMAT(RGBX64);
    MAP_PIXEL_FORMAT(BayerGR8);
    MAP_PIXEL_FORMAT(BayerGR10);
    MAP_PIXEL_FORMAT(BayerGR12);
    MAP_PIXEL_FORMAT(BayerGR14);
    MAP_PIXEL_FORMAT(BayerGR16);
    MAP_PIXEL_FORMAT(BayerRG8);
    MAP_PIXEL_FORMAT(BayerRG10);
    MAP_PIXEL_FORMAT(BayerRG12);
    MAP_PIXEL_FORMAT(BayerRG14);
    MAP_PIXEL_FORMAT(BayerRG16);
    MAP_PIXEL_FORMAT(BayerGB8);
    MAP_PIXEL_FORMAT(BayerGB10);
    MAP_PIXEL_FORMAT(BayerGB12);
    MAP_PIXEL_FORMAT(BayerGB14);
    MAP_PIXEL_FORMAT(BayerGB16);
    MAP_PIXEL_FORMAT(BayerBG8);
    MAP_PIXEL_FORMAT(BayerBG10);
    MAP_PIXEL_FORMAT(BayerBG12);
    MAP_PIXEL_FORMAT(BayerBG14);
    MAP_PIXEL_FORMAT(BayerBG16);
    MAP_PIXEL_FORMAT(BiColorRGBG8);
    MAP_PIXEL_FORMAT(BiColorRGBG10);
    MAP_PIXEL_FORMAT(BiColorRGBG12);
    MAP_PIXEL_FORMAT(BiColorGRGB8);
    MAP_PIXEL_FORMAT(BiColorGRGB10);
    MAP_PIXEL_FORMAT(BiColorGRGB12);
    MAP_PIXEL_FORMAT(BiColorBGRG8);
    MAP_PIXEL_FORMAT(BiColorBGRG10);
    MAP_PIXEL_FORMAT(BiColorBGRG12);
    MAP_PIXEL_FORMAT(BiColorGBGR8);
    MAP_PIXEL_FORMAT(BiColorGBGR10);
    MAP_PIXEL_FORMAT(BiColorGBGR12);
    MAP_PIXEL_FORMAT(Raw);
    MAP_PIXEL_FORMAT(Jpeg);
    }
#undef MAP_PIXEL_FORMAT
    return GraphicsImagePixelFormat::Unknown;
}

[[nodiscard]] GraphicsImageBitAlignment graphicsBitAlignment(
    const Framegrabber::BitAlignment alignment) noexcept
{
    switch (alignment)
    {
    case Framegrabber::BitAlignment::LeastSignificant:
        return GraphicsImageBitAlignment::LeastSignificant;
    case Framegrabber::BitAlignment::MostSignificant:
        return GraphicsImageBitAlignment::MostSignificant;
    case Framegrabber::BitAlignment::Packed:
        return GraphicsImageBitAlignment::Packed;
    }
    return GraphicsImageBitAlignment::Packed;
}
}

namespace FramegrabberGraphicsImageAdapter
{
GraphicsImage wrapImage(
    const Framegrabber::Image& image,
    const std::size_t frameSeq) noexcept
{
    GraphicsImage frame;
    frame.storage = image.storage;
    frame.size = image.size;
    frame.width = image.width;
    frame.height = image.height;
    frame.stride = image.stride;
    frame.bitsPerPixel = image.bitsPerPixel;
    frame.sourceFormat = image.fgFormat;
    frame.pixelFormat = graphicsPixelFormat(image.pixelFormat);
    frame.bitAlignment = graphicsBitAlignment(image.bitAlignment);
    frame.frameSequence = frameSeq;
    return frame;
}

namespace {

class ReadyGuard final
{
public:
    ReadyGuard(Framegrabber* framegrabber, const unsigned int dmaIndex) noexcept
        : _framegrabber(framegrabber), _dmaIndex(dmaIndex) {}
    ~ReadyGuard() { if (_framegrabber) _framegrabber->ready(_dmaIndex); }

private:
    Framegrabber* _framegrabber;
    unsigned int _dmaIndex;
};

} // namespace

GraphicsImageFrameStream::GraphicsImageFrameStream(
    Framegrabber* framegrabber,
    GraphicsFrameCallback callback)
    : _framegrabber(framegrabber), _callback(std::move(callback))
{
    if (!_framegrabber || !_callback)
    {
        return;
    }

    const auto callbackToken = _callbackGate.token();
    _grabCallbackId = _framegrabber->registerGrabCallback(
        [this, callbackToken](const Framegrabber::Image& image, const std::size_t sequence) {
            GraphicsFrameCallbackGate::Lease lease(callbackToken);
            if (!lease) return;
            ReadyGuard ready(_framegrabber, image.dmaIndex);
            try
            {
                GraphicsImage graphicsImage = wrapImage(image, sequence);
                if (!graphicsImage.isValid()) return;

                GraphicsFrame frame;
                frame.setImage(std::move(graphicsImage));
                _callback(std::move(frame), image.dmaIndex);
            }
            catch (const std::exception& error)
            {
                FramegrabberSystem::syslog(
                    std::string("Framegrabber GraphicsFrame callback failed: ") + error.what(), true);
            }
            catch (...)
            {
                FramegrabberSystem::syslog(
                    "Framegrabber GraphicsFrame callback failed with an unknown exception.", true);
            }
        });
}

GraphicsImageFrameStream::~GraphicsImageFrameStream()
{
    _callbackGate.beginShutdown();
    if (_framegrabber && _grabCallbackId != 0U)
    {
        _framegrabber->deregisterGrabCallback(_grabCallbackId);
    }
    _callbackGate.waitForDrain();
}
}
