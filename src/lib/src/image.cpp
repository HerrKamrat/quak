#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image ImageReader::read(const Uint8DataView& view) {
    int xOut, yOut, cOut;

    const int comp = 4;
    auto d = stbi_load_from_memory(view.data(), view.size(), &xOut, &yOut, &cOut, comp);
    const int l = xOut * yOut * comp;

    uint8_t w = xOut;
    uint8_t h = yOut;
    PixelFormat f = PixelFormat::RGBA;

    Buffer b;
    b.reset((Buffer::pointer)d, l, l, stbi_image_free);
    return {w, h, f, std::move(b)};
}