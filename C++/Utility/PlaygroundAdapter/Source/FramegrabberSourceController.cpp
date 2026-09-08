#include "FramegrabberSourceController.h"

#include "Utility/PlaygroundAdapter/FramegrabberGraphicsFrameStream.h"
#include "SessionFrame.h"

#include <QDebug>

#include <cstddef>
#include <utility>

FramegrabberSourceController::FramegrabberSourceController(
    Framegrabber* framegrabber,
    QObject* parent)
    : AbstractSourceController(parent),
      _framegrabber(framegrabber)
{
    registerCallbacks();
}

FramegrabberSourceController::~FramegrabberSourceController()
{
    stop();
    deregisterCallbacks();
}

void FramegrabberSourceController::start()
{
    if (_framegrabber)
    {
        _framegrabber->grab();
    }
}

void FramegrabberSourceController::stop()
{
    if (_framegrabber)
    {
        _framegrabber->stop();
    }
    _isGrabbing.store(false, std::memory_order_release);
}

bool FramegrabberSourceController::isGrabbing() const
{
    return _isGrabbing.load(std::memory_order_acquire);
}

void FramegrabberSourceController::setFrameConsumer(FrameConsumer consumer)
{
    _frameConsumer = std::move(consumer);
}

void FramegrabberSourceController::registerCallbacks()
{
    if (!_framegrabber)
    {
        return;
    }

    _statusCallbackId = _framegrabber->registerStatusCallback(
        [this](const Framegrabber::Status status, const bool on)
        {
            if (status == Framegrabber::GrabbingStatus)
            {
                _isGrabbing.store(on, std::memory_order_release);
            }
        });

    _graphicsStream = std::make_unique<FramegrabberGraphicsFrameStream>(
        _framegrabber,
        [this](GraphicsFrame&& payload, const unsigned int sourceIndex)
        {
            if (!_frameConsumer) return;
            SessionFrame frame;
            frame.payload = std::move(payload);
            frame.frameSeq = frame.payload.image.has_value()
                ? frame.payload.image->frameSequence
                : 0U;
            _frameConsumer(std::move(frame), sourceIndex);
        });
}

void FramegrabberSourceController::deregisterCallbacks()
{
    if (!_framegrabber)
    {
        return;
    }
    _graphicsStream.reset();
    if (_statusCallbackId != 0)
    {
        _framegrabber->deregisterStatusCallback(_statusCallbackId);
        _statusCallbackId = 0;
    }
}
