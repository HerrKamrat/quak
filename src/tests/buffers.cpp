#include <Remotery.h>
#include <catch2/catch.hpp>

#include <chrono>
#include <thread>

#include "buffer.hpp"
#include "data_view.hpp"

auto createBuffer(size_t size) {
    std::unique_ptr<Buffer> buffer(new Buffer(size));
    for (int i = 0; i < size; i++) {
        (*buffer)[i] = i;
        buffer->at(i) = i;
    }

    return buffer;
}

template <typename T0, typename T1>
T0 r_c(T1 p) {
    return reinterpret_cast<T0>(p);
}

TEST_CASE("Buffer and DataView", "[buffer]") {
    const auto size = 101;
    auto buffer = createBuffer(size);

    for (int i = 0; i < size; i++) {
        (*buffer)[i] = i;
        buffer->at(i) = i;
    }


	REQUIRE(size == std::distance(buffer->begin(), buffer->end()));
    REQUIRE(size == std::distance(buffer->cbegin(), buffer->cend()));
    REQUIRE(size == std::distance(buffer->rbegin(), buffer->rend()));
    REQUIRE(size == std::distance(buffer->crbegin(), buffer->crend()));

    REQUIRE(buffer->size() == size);
    for (int i = 0; i < size; i++) {
        REQUIRE(buffer->at(i) == i);
    }

    {
        Uint8DataView u8 = {buffer->data(), buffer->size()};
        Uint16DataView u16 = {buffer->data(), buffer->size()};
        Uint32DataView u32 = {buffer->data(), buffer->size()};
        Uint64DataView u64 = {buffer->data(), buffer->size()};

        Float32DataView f32 = {buffer->data(), buffer->size()};
        Float64DataView f64 = {buffer->data(), buffer->size()};

        REQUIRE(u8.size() == (size / (8 / 8)));
        REQUIRE(u16.size() == (size / (16 / 8)));
        REQUIRE(u32.size() == (size / (32 / 8)));
        REQUIRE(u64.size() == (size / (64 / 8)));

        REQUIRE(f32.size() == (size / (32 / 8)));
        REQUIRE(f64.size() == (size / (64 / 8)));
    }

    {
        Uint8DataView u8 = {r_c<Uint8DataView::pointer>(buffer->data()), buffer->size()};
        Uint16DataView u16 = {r_c<Uint16DataView::pointer>(buffer->data()), buffer->size() / 2};
        Uint32DataView u32 = {r_c<Uint32DataView::pointer>(buffer->data()), buffer->size() / 4};
        Uint64DataView u64 = {r_c<Uint64DataView::pointer>(buffer->data()), buffer->size() / 8};

        Float32DataView f32 = {buffer->data(), buffer->size()};
        Float64DataView f64 = {buffer->data(), buffer->size()};

        REQUIRE(u8.size() == (size / (8 / 8)));
        REQUIRE(u16.size() == (size / (16 / 8)));
        REQUIRE(u32.size() == (size / (32 / 8)));
        REQUIRE(u64.size() == (size / (64 / 8)));

        REQUIRE(f32.size() == (size / (32 / 8)));
        REQUIRE(f64.size() == (size / (64 / 8)));
    }

    {
        Uint8DataView u8 = {buffer->data(), buffer->size()};
        Uint16DataView u16 = {buffer->data(), buffer->size()};
        Uint32DataView u32 = {buffer->data(), buffer->size()};
        Uint64DataView u64 = {buffer->data(), buffer->size()};

        Float32DataView f32 = {buffer->data(), buffer->size()};
        Float64DataView f64 = {buffer->data(), buffer->size()};


		for (auto& it : u8) {
            it = 0;
		}

		u8[8] = 0xff;

        REQUIRE(u8[0] == 0);
        REQUIRE(u16[0] == 0);
        REQUIRE(u32[0] == 0);
        REQUIRE(u64[0] == 0);
        REQUIRE(f32[0] == 0);
        REQUIRE(f64[0] == 0);

		REQUIRE(u16[4] == 0xff);
        REQUIRE(u32[2] == 0xff);
        REQUIRE(u64[1] == 0xff);
    }
}
