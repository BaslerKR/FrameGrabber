#pragma once

/**
 * @file FramegrabberGraphicsImageStream.h
 * @brief Owns Framegrabber grab callbacks and publishes owned GraphicsFrame values.
 */

#include "engine/GraphicsFrameAdapter.h"

#include <memory>

class Framegrabber;

/** Owns frame-grabber callback registration and emits only owned GraphicsFrame values. */
class FramegrabberGraphicsImageStream final
{
public:
    FramegrabberGraphicsImageStream(Framegrabber* framegrabber, GraphicsFrameCallback callback);
    ~FramegrabberGraphicsImageStream();

    FramegrabberGraphicsImageStream(const FramegrabberGraphicsImageStream&) = delete;
    FramegrabberGraphicsImageStream& operator=(const FramegrabberGraphicsImageStream&) = delete;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
