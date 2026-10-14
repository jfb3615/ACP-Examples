#pragma once
#include <array>
#include <tuple>

constexpr int kImageWidth{800};
constexpr int kImageHeight{800};

using Triplet    = std::tuple<int,int,int>;
using PixelArray = std::array<Triplet, kImageWidth * kImageHeight>;

void createImage(PixelArray &);
void render (const PixelArray & );

