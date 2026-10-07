#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

namespace mk64_3ds {
constexpr unsigned kSoftWidth = 32, kSoftHeight = 24;
using SoftPixels = std::array<uint32_t, kSoftWidth * kSoftHeight>;
// Brighten the displayed backdrop by ~40% without changing its blur/colors:
// baseline transmission is (255-204)/255; raise that, not the black alpha.
// Nearest 8-bit alpha is 184: 71/51 = 1.392 (39.2% brighter).
constexpr uint8_t kMenuBackgroundScrimAlpha = 184;
// Average the whole source (not sparse samples), then two low-resolution box
// passes. Fixed scratch storage; no full-resolution copy or frame-time blur.
template <typename Pixel>
SoftPixels MakeSoftBackdrop(unsigned width, unsigned height, Pixel pixel) {
    SoftPixels colors{}, scratch{};
    if (!width || !height) return colors;
    for (unsigned y = 0; y < kSoftHeight; ++y) {
        for (unsigned x = 0; x < kSoftWidth; ++x) {
            unsigned sums[3]{}, count = 0;
            const unsigned x0 = x * width / kSoftWidth;
            const unsigned y0 = y * height / kSoftHeight;
            const unsigned x1 = std::max(x0 + 1, (x + 1) * width / kSoftWidth);
            const unsigned y1 = std::max(y0 + 1, (y + 1) * height / kSoftHeight);
            for (unsigned sy = y0; sy < y1; ++sy) {
                for (unsigned sx = x0; sx < x1; ++sx) {
                    const auto rgba = pixel(std::min(sx, width - 1), std::min(sy, height - 1));
                    for (unsigned c = 0; c < 3; ++c) sums[c] += (rgba >> (c * 8)) & 255;
                    ++count;
                }
            }
            colors[y * kSoftWidth + x] = 0xff000000U | sums[0] / count |
                ((sums[1] / count) << 8) | ((sums[2] / count) << 16);
        }
    }
    for (unsigned pass = 0; pass < 2; ++pass) {
        for (int y = 0; y < static_cast<int>(kSoftHeight); ++y) {
            for (int x = 0; x < static_cast<int>(kSoftWidth); ++x) {
                unsigned sums[3]{}, count = 0;
                for (int sy = std::max(0, y - 3); sy <= std::min(23, y + 3); ++sy) {
                    for (int sx = std::max(0, x - 3); sx <= std::min(31, x + 3); ++sx) {
                        const auto rgba = colors[sy * kSoftWidth + sx];
                        for (unsigned c = 0; c < 3; ++c) sums[c] += (rgba >> (c * 8)) & 255;
                        ++count;
                    }
                }
                scratch[y * kSoftWidth + x] = 0xff000000U | sums[0] / count |
                    ((sums[1] / count) << 8) | ((sums[2] / count) << 16);
            }
        }
        colors = scratch;
    }
    return colors;
}

constexpr std::array<unsigned, 16> kMenuCourseOrder{
    8, 9, 6, 11, 10, 5, 1, 0, 14, 12, 7, 2, 18, 4, 3, 13
};
struct ConveyorSlot { unsigned map; float x; };
constexpr unsigned kMenuConveyorSpacing = 192;
constexpr uint64_t kMenuConveyorPeriodMs = kMenuCourseOrder.size()*12000;
inline ConveyorSlot MenuConveyorSlot(uint64_t elapsedMs, unsigned visibleSlot) {
    // Four cups in race order, 16 pixels/second. Luigi starts centered;
    // Rainbow Road precedes it, Moo Moo Farm follows it. Arenas are not cups.
    const float pixels = static_cast<float>(elapsedMs % kMenuConveyorPeriodMs) * 0.016f;
    const unsigned first = static_cast<unsigned>(pixels / kMenuConveyorSpacing);
    // Slot 0 starts offscreen: it must be the previous map, not Luigi Raceway.
    // The first visible slot is then course 0 on the very first title frame.
    return {static_cast<unsigned>((first + visibleSlot + kMenuCourseOrder.size()-1) % kMenuCourseOrder.size()),
            64.0f-static_cast<float>(kMenuConveyorSpacing) - (pixels - first * kMenuConveyorSpacing) +
                visibleSlot * static_cast<float>(kMenuConveyorSpacing)};
}
inline bool IdleMenuBackdrop(int gameState, int menuSelection, bool occupied) {
    return !occupied && gameState != 4 && gameState != 5 && gameState != 9 &&
           menuSelection >= 10 && menuSelection <= 13;
}
} // namespace mk64_3ds
