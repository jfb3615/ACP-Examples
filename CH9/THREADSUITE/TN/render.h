#pragma once
#include <array>
#include <cstdint>
#include <QColor>
constexpr int kImageWidth = 800;
constexpr int kImageHeight = 800;
constexpr int kQuadrantSize = kImageWidth / 2;

using PixelArray = std::array<std::uint32_t, kImageWidth * kImageHeight>;

void render (const PixelArray & );

