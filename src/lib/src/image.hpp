#pragma once

#include <cstdint>

#include "buffer.hpp"
#include "data_view.hpp"

enum class PixelFormat {
    RGBA,
};



class Image {
  public:
    Image(uint8_t w, uint8_t h, PixelFormat format, Buffer&& buffer)
        : m_width(w), m_height(h), m_format(format), m_buffer(std::move(buffer)) {
    }

    const char* data() const {
        return m_buffer.data();
    }

    uint8_t width() const {
        return m_width;
    }
    
	uint8_t height() const {
        return m_height;
    }
    
	/** returns the bytes per row */
	uint8_t stride() const {
		//TODO 4 = bbp
        return m_width * 4;
    }

  protected:
  private:
    uint8_t m_width;
    uint8_t m_height;
    PixelFormat m_format;
    Buffer m_buffer;
};

class ImageReader {
  public:
    static Image read(const Uint8DataView& view);
};
